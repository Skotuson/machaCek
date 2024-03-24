#include <functional>
#include <iostream>
#include <cassert>
#include <vector>
#include <random>
#include <ctime>

const size_t DICE_MAX = 6;
const size_t DICE_MIN = 1;
const size_t UNROLLED = -1;
const size_t MACHACEK = 21;
const size_t PLAYER_HEALTH = 4;

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

    friend std::ostream &operator<<(std::ostream &os, const Dice &d)
    {
        return os << d.count();
    }

private:
    size_t m_Roll = UNROLLED;
};

struct Throw
{
    Throw(const Dice &d1, const Dice &d2, MComparator comp)
        : m_First(d1),
          m_Second(d2),
          m_Comp(comp)
    {
    }

    void reroll(void)
    {
        m_First.roll();
        m_Second.roll();
    }

    bool operator<(const Throw &t) const
    {
        return m_Comp(*this, t);
    }

    bool operator==(const Throw &t) const
    {
        return !m_Comp(*this, t) && !m_Comp(t, *this);
    }

    bool operator!=(const Throw &t) const
    {
        return !(*this == t);
    }

    bool operator<=(const Throw &t) const
    {
        return *this < t || *this == t;
    }

    size_t native(void) const
    {
        if (first() == second())
            return first();
        return 0;
    }

    size_t value(void) const
    {
        if (native())
            return native();
        return std::max(first(), second()) * 10 + std::min(first(), second());
    }

    friend std::ostream &operator<<(std::ostream &os, const Throw &t)
    {
        return os << (t.native() ? std::to_string(t.native()) + " natives" : (t.value() == MACHACEK ? "MACHACEK" : std::to_string(t.value())));
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
    // Machacek
    if (t1.value() == MACHACEK)
    {
        return t2.value() != MACHACEK;
    }

    else if (t2.value() == MACHACEK)
    {
        return t1.value() != MACHACEK;
    }

    // Native case
    size_t c1, c2;
    if ((c1 = t1.native()) && (c2 = t2.native()))
    {
        return c1 < c2;
    }

    else if (t1.native() && !t2.native())
    {
        return false;
    }

    else if (!t1.native() && t2.native())
    {
        return true;
    }

    return t1.value() < t2.value();
}

const std::vector<Throw> ALL_THROWS = {
    Throw(Dice(3), Dice(1), MACHACEK_COMPARATOR),
    Throw(Dice(3), Dice(2), MACHACEK_COMPARATOR),
    Throw(Dice(4), Dice(1), MACHACEK_COMPARATOR),
    Throw(Dice(4), Dice(2), MACHACEK_COMPARATOR),
    Throw(Dice(4), Dice(3), MACHACEK_COMPARATOR),
    Throw(Dice(5), Dice(1), MACHACEK_COMPARATOR),
    Throw(Dice(5), Dice(1), MACHACEK_COMPARATOR),
    Throw(Dice(5), Dice(2), MACHACEK_COMPARATOR),
    Throw(Dice(5), Dice(3), MACHACEK_COMPARATOR),
    Throw(Dice(5), Dice(4), MACHACEK_COMPARATOR),
    Throw(Dice(6), Dice(1), MACHACEK_COMPARATOR),
    Throw(Dice(6), Dice(2), MACHACEK_COMPARATOR),
    Throw(Dice(6), Dice(3), MACHACEK_COMPARATOR),
    Throw(Dice(6), Dice(4), MACHACEK_COMPARATOR),
    Throw(Dice(6), Dice(5), MACHACEK_COMPARATOR),
    Throw(Dice(1), Dice(1), MACHACEK_COMPARATOR),
    Throw(Dice(2), Dice(2), MACHACEK_COMPARATOR),
    Throw(Dice(3), Dice(3), MACHACEK_COMPARATOR),
    Throw(Dice(4), Dice(4), MACHACEK_COMPARATOR),
    Throw(Dice(5), Dice(5), MACHACEK_COMPARATOR),
    Throw(Dice(6), Dice(6), MACHACEK_COMPARATOR),
    Throw(Dice(2), Dice(1), MACHACEK_COMPARATOR)};

struct Player
{
    Player(size_t health = PLAYER_HEALTH, const Throw &thr = Throw(Dice(), Dice(), MACHACEK_COMPARATOR))
        : m_Health(health),
          m_Throw(thr)
    {
    }

    void roll(void)
    {
        m_Throw.reroll();
    }

    const Throw &getThrow(void) const
    {
        return m_Throw;
    }

    size_t health(void)
    {
        return m_Health;
    }

    size_t hit(void)
    {
        if (m_Health)
        {
            return m_Health--;
        }
        return m_Health;
    }

    friend std::ostream &operator<<(std::ostream &os, const Player &p)
    {
        return os << p.m_Throw;
    }

private:
    size_t m_Health;
    Throw m_Throw;
};

int main(void)
{
    assert(Throw(Dice(1), Dice(3), MACHACEK_COMPARATOR) < Throw(Dice(2), Dice(3), MACHACEK_COMPARATOR));
    assert(Throw(Dice(4), Dice(5), MACHACEK_COMPARATOR) < Throw(Dice(5), Dice(6), MACHACEK_COMPARATOR));
    assert(Throw(Dice(1), Dice(1), MACHACEK_COMPARATOR) < Throw(Dice(4), Dice(4), MACHACEK_COMPARATOR));
    assert(Throw(Dice(6), Dice(6), MACHACEK_COMPARATOR) < Throw(Dice(1), Dice(2), MACHACEK_COMPARATOR));
    assert(!(Throw(Dice(1), Dice(1), MACHACEK_COMPARATOR) < Throw(Dice(5), Dice(4), MACHACEK_COMPARATOR)));
    assert(!(Throw(Dice(4), Dice(4), MACHACEK_COMPARATOR) < Throw(Dice(5), Dice(4), MACHACEK_COMPARATOR)));
    assert(Throw(Dice(6), Dice(3), MACHACEK_COMPARATOR) < Throw(Dice(1), Dice(2), MACHACEK_COMPARATOR));
    assert(Throw(Dice(5), Dice(3), MACHACEK_COMPARATOR) < Throw(Dice(1), Dice(2), MACHACEK_COMPARATOR));

    std::string choice = "";
    Player player;
    Player opponent;

    player.roll();
    opponent.roll();

    bool playerTurn = false;
    bool playerLying = false;

    Throw lastRound = opponent.getThrow();

    while (choice != "stop")
    {
        if (!player.health())
        {
            std::cout << "You lost!" << std::endl;
            break;
        }

        if (!opponent.health())
        {
            std::cout << "You won!" << std::endl;
            break;
        }

        if (!playerTurn)
        {
            Throw opponentThrow = opponent.getThrow();
            Player opponentCpy = opponent;
            while (opponentThrow < lastRound)
            {
                opponentCpy.roll();
                opponentThrow = opponentCpy.getThrow();
            }
            std::cout << "Opponent says: \"I have " << opponentThrow << "\"" << std::endl;
            std::cout << "-> b(ullshit), t(rust): ";

            std::getline(std::cin >> std::ws, choice);

            if (choice.size() == 1 && (choice[0] == 'b' || choice[0] == 't'))
            {
                if (choice[0] == 'b' && opponentThrow == opponent.getThrow())
                {
                    std::cout << "Opponent really did throw that (-1 for you)" << std::endl;
                    player.hit();
                    lastRound = Throw(Dice(3), Dice(1), MACHACEK_COMPARATOR);
                }

                else if (choice[0] == 'b' && opponentThrow != opponent.getThrow())
                {
                    std::cout << "Opponent lied (-1 for him)" << std::endl;
                    opponent.hit();
                    lastRound = Throw(Dice(3), Dice(1), MACHACEK_COMPARATOR);
                }
                opponent.roll();
                playerTurn = true;
            }
        }

        else
        {
            std::cout << "You rolled " << player << "." << std::endl;
            std::cout << "-> t(ell the truth), f(ake)[1-6][1-6]: ";
            std::getline(std::cin >> std::ws, choice);

            if (choice.size() == 1 && choice[0] == 't')
            {
                if (player.getThrow() < lastRound)
                {
                    std::cout << "You told the truth. The truth is, the throw isn't enough (-1 for you)" << std::endl;
                    player.hit();
                    lastRound = Throw(Dice(3), Dice(1), MACHACEK_COMPARATOR);
                    continue;
                }

                int smaller = 0;
                for (const auto &thr : ALL_THROWS)
                {
                    if (thr == player.getThrow())
                        break;
                    smaller++;
                }

                std::random_device device;
                std::mt19937 gen(device());
                std::uniform_int_distribution<int> dist(0, ALL_THROWS.size());

                bool opponentAccuse = dist(gen) < smaller;

                if (opponentAccuse && playerLying)
                {
                    std::cout << "Liar liar, pants on fire (-1 for you)" << std::endl;
                    player.hit();
                    lastRound = Throw(Dice(3), Dice(1), MACHACEK_COMPARATOR);
                }

                else if (opponentAccuse && !playerLying)
                {
                    std::cout << "Opponent just couldn't believe it (-1 for him)" << std::endl;
                    opponent.hit();
                    lastRound = Throw(Dice(3), Dice(1), MACHACEK_COMPARATOR);
                }

                player.roll();
                playerTurn = false;
                playerLying = false;
            }

            else if (choice.size() == 3 && choice[0] == 'f' && isdigit(choice[1]) && isdigit(choice[2]))
            {
                int n = std::stoi(choice.substr(1));
                if (n >= 11 && n <= 66)
                {
                    player = Player(player.health(), Throw(Dice(std::floor(n / 10)), Dice(n % 10), MACHACEK_COMPARATOR));
                    playerLying = true;
                }
            }
        }
    }
    return 0;
}