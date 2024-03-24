#include <iostream>
#include <functional>
#include <random>

const size_t DICE_MAX = 6;
const size_t DICE_MIN = 1;
const size_t UNROLLED = -1;
const size_t MACHACEK = 21;

struct Throw;

using MComparator = std::function<bool(const Throw &t1, const Throw &t2)>;

struct Dice
{

    Dice(size_t roll = UNROLLED)
        : m_Roll(roll)
    {
    }

    size_t count(void) const
    {
        return m_Roll;
    }

    void roll(void)
    {
        std::random_device device;
        std::mt19937 gen(device());
        std::uniform_int_distribution<int> dist(DICE_MIN, DICE_MAX);
        m_Roll = dist(gen);
    }

private:
    size_t m_Roll = UNROLLED;
};

struct Throw
{
    Throw(Dice &d1, Dice &d2, MComparator comp)
        : m_First(d1),
          m_Second(d2),
          m_Comp(comp)
    {
    }

    bool operator<(const Throw &t)
    {
        return m_Comp(*this, t);
    }

    size_t native(void) const
    {
        if (first() == second())
            return first();
        return 0;
    }

    size_t value(void)
    {
        return std::max(first(), second()) * 10 + std::min(first(), second());
    }

private:
    size_t first(void) const
    {
        return m_First.count();
    }

    size_t second(void) const
    {
        return m_Second.count();
    }

    Dice m_First;
    Dice m_Second;
    MComparator m_Comp;
};

bool MACHACEK_COMPARATOR(const Throw &t1, const Throw &t2)
{
    //Machacek
    

    // Native case
    size_t c1, c2;
    if ((c1 = t1.native()) && (c2 = t2.native()))
    {
        return c1 < c2;
    }
}

int main(void)
{
    return 0;
}