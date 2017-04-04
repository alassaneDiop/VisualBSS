#include <bandit/bandit.h>
#include <stationsfilter.h>
#include <station.h>

using namespace bandit;
using namespace snowhouse;

go_bandit([](){
    describe("test stationfilter", [](){

        QVector<Station> stations;

        /* sn : create some stations for the tests*/
        Station s1;
        s1.tripsFlow = 51; //this station have to be accept (51 >= 51)
        stations.append(s1);

        Station s2; //this station is bad (50 < 51)
        s2.tripsFlow = 50;
        stations.append(s2);

        it("Trips flow min", [&](){ // test the minimum (here 51)
            StationsFilterParams params;
            params.minTripsFlow = 51;
            params.maxTripsFlow = 60;
            const StationsFilter stationFilter = StationsFilter(params);
            QVector<Station> outStations = stationFilter.filter(stations);
            AssertThat(outStations.size(), Equals(1));
            if(outStations.size() == 1)
                AssertThat(outStations[0].id, Equals(s1.id));
        });

        Station s3; // this one is good (90 <= 90)
        s3.tripsFlow = 90;
        stations.append(s3);

        Station s4; // but not this one (91 > 90)
        s4.tripsFlow = 91;
        stations.append(s4);

        it("Trips flow max", [&](){ // test the maximum Trips Flow (here 90)
            StationsFilterParams params;
            params.minTripsFlow = 60;
            params.maxTripsFlow = 90;
            const StationsFilter stationFilter = StationsFilter(params);
            QVector<Station> outStations = stationFilter.filter(stations);
            AssertThat(outStations.size(), Equals(1));
            if(outStations.size() == 1)
                AssertThat(outStations[0].id, Equals(s3.id));
        });

        it("Trips flow invert min and max", [&](){ // test when we invert the min and the max
            StationsFilterParams params;
            params.minTripsFlow = 90;
            params.maxTripsFlow = 60;
            const StationsFilter stationFilter = StationsFilter(params);
            QVector<Station> outStations = stationFilter.filter(stations);
            AssertThat(outStations.size(), Equals(0));
        });
    });
});
