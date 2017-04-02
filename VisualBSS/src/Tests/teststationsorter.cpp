#include <bandit/bandit.h>
#include <stationssorter.h>
#include <station.h>
#include <trip.h>

using namespace bandit;
using namespace snowhouse;

go_bandit([](){
    describe("test stationssorter", [](){

        QVector<Station> stations;

        /* sn : create some stations for the tests*/

        Station s1;
        s1.id = 1;
        for(int i = 0; i < 3; i++){ //ARRIVAL
            Trip trip;
            trip.id = i;
            s1.appendArrival(trip);
        }
        for(int i = 0; i < 5; i++){ //CYCLE
            Trip trip;
            trip.id = i;
            s1.appendCycle(trip);
        }
        for(int i = 0; i < 7; i++){ //DEPARTURES
            Trip trip;
            trip.id = i;
            s1.appendDeparture(trip);
        }
        s1.avgTripDuration = 40; //DURATION
        s1.avgTripDistance = 10; //DISTANCE
        stations.append(s1);

        Station s2;
        s2.id = 2;
        for(int i = 0; i < 5; i++){ //ARRIVAL
            Trip trip;
            trip.id = i;
            s2.appendArrival(trip);
        }
        for(int i = 0; i < 7; i++){ //CYCLE
            Trip trip;
            trip.id = i;
            s2.appendCycle(trip);
        }
        for(int i = 0; i < 5; i++){ //DEPARTURES
            Trip trip;
            trip.id = i;
            s2.appendDeparture(trip);
        }
        s2.avgTripDuration = 20; //DURATION
        s2.avgTripDistance = 30; //DISTANCE
        stations.append(s2);

        Station s3;
        s3.id = 3;
        for(int i = 0; i < 7; i++){ //ARRIVAL
            Trip trip;
            trip.id = i;
            s3.appendArrival(trip);
        }
        for(int i = 0; i < 2; i++){ //CYCLE
            Trip trip;
            trip.id = i;
            s3.appendCycle(trip);
        }
        for(int i = 0; i < 2; i++){ //DEPARTURES
            Trip trip;
            trip.id = i;
            s3.appendDeparture(trip);
        }
        s2.avgTripDuration = 0; //DURATION
        s2.avgTripDistance = 50; //DISTANCE
        stations.append(s3);

        const QVector<Station> stationsIn = stations;

        it("ARRIVALS", [&](){
            StationsSorter stationsorter = StationsSorter(bss::ARRIVALS);
            QVector<Station> outStations = stationsorter.sort(stationsIn);
            AssertThat(outStations.size(), Equals(3));
            if(outStations.size() == 3){
                AssertThat(outStations[0].id, Equals(s3.id));
                AssertThat(outStations[1].id, Equals(s2.id));
                AssertThat(outStations[2].id, Equals(s1.id));
            }
        });

        it("CYCLES", [&](){
            StationsSorter stationsorter = StationsSorter(bss::CYCLES);
            QVector<Station> outStations = stationsorter.sort(stationsIn);
            AssertThat(outStations.size(), Equals(3));
            if(outStations.size() == 3){
                AssertThat(outStations[0].id, Equals(s2.id));
                AssertThat(outStations[1].id, Equals(s1.id));
                AssertThat(outStations[2].id, Equals(s3.id));
            }
        });

        it("DEPARTURES", [&](){
            StationsSorter stationsorter = StationsSorter(bss::DEPARTURES);
            QVector<Station> outStations = stationsorter.sort(stationsIn);
            AssertThat(outStations.size(), Equals(3));
            if(outStations.size() == 3){
                AssertThat(outStations[0].id, Equals(s1.id));
                AssertThat(outStations[1].id, Equals(s2.id));
                AssertThat(outStations[2].id, Equals(s3.id));
            }
        });

        it("DISTANCE", [&](){
            StationsSorter stationsorter = StationsSorter(bss::DISTANCE);
            QVector<Station> outStations = stationsorter.sort(stationsIn);
            AssertThat(outStations.size(), Equals(3));
            if(outStations.size() == 3){
                AssertThat(outStations[0].id, Equals(s2.id));
                AssertThat(outStations[1].id, Equals(s1.id));
                AssertThat(outStations[2].id, Equals(s3.id));
            }
        });

        it("DURATION", [&](){
            StationsSorter stationsorter = StationsSorter(bss::DURATION);
            QVector<Station> outStations = stationsorter.sort(stationsIn);
            AssertThat(outStations.size(), Equals(3));
            if(outStations.size() == 3){
                AssertThat(outStations[0].id, Equals(s1.id));
                AssertThat(outStations[1].id, Equals(s2.id));
                AssertThat(outStations[2].id, Equals(s3.id));
            }
        });
    });
});
