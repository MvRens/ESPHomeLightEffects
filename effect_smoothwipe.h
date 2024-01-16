uint32_t sw_transitionLength = 1000;
uint8_t sw_edgeSize = 0;
uint8_t sw_edgeStepSize = 0;
bool sw_reverse = false;
uint32_t sw_startTime = 0;
uint32_t sw_lastProgress = 0;
uint32_t sw_maxProgress = 0;
int sw_lastStoredEdge = -1;

Color* sw_startingColors = NULL;


// transitionLength:    The duration of the transition, in milliseconds.
// edgeSize:            How many LEDs the leading edge is spread across. The larger this value is, the smoother the transition.  
// reverse:             If true, the effect runs from the last to the first LED.
void smoothWipeInit(AddressableLight &it, uint32_t transitionLength, uint8_t edgeSize, bool reverse)
{
    ESP_LOGD("custom", "SmoothWipe build %s %s", __DATE__, __TIME__);

    auto itSize = it.size();

    sw_transitionLength = transitionLength;

    sw_edgeSize = edgeSize;
    sw_edgeStepSize = edgeSize > 0 ? 255 / sw_edgeSize : 255;

    // Keep track of the colors as they were before starting the wipe.
    // I initially used a strategy of keeping track of the edge only, but that is tricky. Feel free to optimize.
    // It only takes 4 bytes per LED so the ESP32 is more than capable of handling this in my scenario.
    if (sw_startingColors != NULL)
        delete[] sw_startingColors;

    sw_startingColors = new Color[itSize];
    for (int i = 0; i < itSize; i++)
        sw_startingColors[i] = it[i].get();

    sw_reverse = reverse;
    sw_startTime = millis();
    sw_lastProgress = 0;

    // Each LED is assigned 255 steps so regardless of the interval or transition length
    // we get a smoothed result. The edgeSize is added twice to account for lead-in and out. 
    sw_maxProgress = (itSize + (2 * sw_edgeSize)) * 255;
}


bool smoothWipe(AddressableLight &it, Color current_color)
{
    uint32_t now = millis();

    // Determine how far we're supposed to be
    float progressPercentage = (float)(now - sw_startTime) / sw_transitionLength;
    uint32_t progress = (uint32_t)(progressPercentage * sw_maxProgress);

    if (progress >= sw_maxProgress)
        progress = sw_maxProgress;

    // Determine where the edge was, and where it now starts and stops
    int lastEdgeEnd = sw_lastProgress / 255;
    int lastEdgeStart = lastEdgeEnd - sw_edgeSize;
    int edgeEnd = progress / 255;
    int edgeStart = edgeEnd - sw_edgeSize;
    auto itSize = it.size();


    if (lastEdgeStart < 0)
        lastEdgeStart = 0;

    if (edgeEnd > itSize)
        edgeEnd = itSize;

    for (int i = lastEdgeStart; i < edgeEnd; i++)
    {
        int ledIndex = sw_reverse ? itSize - i - 1 : i;

        if (i < edgeStart)
        {
            // Behind the edge, set to the current color
            it[ledIndex] = current_color;
        }
        else
        {
            // Start fading from the start of the edge
            uint8_t fadeAmount = (i - edgeStart) * sw_edgeStepSize;
            it[ledIndex] = current_color.gradient(sw_startingColors[ledIndex], fadeAmount);
        } 
    }

    sw_lastProgress = progress;
    return progress == sw_maxProgress;
}