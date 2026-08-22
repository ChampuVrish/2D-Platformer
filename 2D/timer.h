#pragma once

class Timer
{
    float length;
    float Time;
    bool Timeout;

public:

    Timer(float length)
        : length(length), Time(0), Timeout(false)
    {
    }

    bool step(float deltaTime)
    {
        Time += deltaTime;

        if (Time > length)
        {
            Time -= length;
            Timeout = true;
            return true;
        }
        return false;
    }

    bool isTimeout() const
    {
        return Timeout;
    }

    float getTime() const
    {
        return Time;
    }

    float getLength() const
    {
        return length;
    }

    void reset()
    {
        Time = 0;
        Timeout = false;
    }
};