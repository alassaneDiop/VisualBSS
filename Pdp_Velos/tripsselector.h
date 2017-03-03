#ifndef SELECTOR_H
#define SELECTOR_H


#include <QSet>

namespace bss {
class TripsSelector;
}

class Station;
class Trip;
class TripsSelector
{
    struct SelectionParams
    {
        QSet<const Station*> stations;
        int startHour = -1;
        int endHour = -1;
    };

public:
    TripsSelector();
    TripsSelector(const SelectionParams& params);

    inline const SelectionParams& getParameters() const { return m_params; }
    inline void setParams(const SelectionParams& params) { m_params = params; }

    QSet<const Trip*> selectTripsFrom(const QSet<const Trip*>& trips) const;

private:
    SelectionParams m_params;
};

#endif // SELECTOR_H
