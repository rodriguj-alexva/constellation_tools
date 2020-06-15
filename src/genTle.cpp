/*
 * generate generic TLE for circular orbit : input are name, altitude, inclination, RAAN, optionally epoch
 *
 * JMR 6/15/2020
 */

#include <iostream>
#include <sstream>
#include <ctime>
#include <string>
#include <math.h>

#include <string.h>

#include <quicktle/node.h>

#define RADIUS_EARTH 6378.14
#define MU 398601.2
#define GM 3.986E14 // G * Mass_earth

int outputTle(
	const char * satelliteName,
	const char * satelliteNumber,
	double semiMajorAxis, // km
	double inclination, // degrees
	double RAAN,
	double eccentricity, // 0 to 1
	double argumentOfPerigee, // degrees
	double meanAnomaly,
	double epoch
	)
{
	// convert semi major axis length to Mean Motion
	double meanMotion = (86400.0/(2*M_PI)) * sqrt( MU / pow(semiMajorAxis, 3));
//	printf("mean motion %f \n", meanMotion);
	
    quicktle::Node node;              // create "empty" node

    node.setSatelliteName(satelliteName);    // set satellite name
    node.setSatelliteNumber(satelliteNumber);// NORAD number
    node.setDesignator(satelliteNumber);    // International designator
    
    node.setClassification('U');     // classification
    node.setPreciseEpoch(epoch);
    
    node.setInclination(inclination);
    node.setRightAscensionAscendingNode(RAAN);
    node.setEccentricity(eccentricity);
    node.set_omega(argumentOfPerigee);		// Argument of Perigee
    node.set_M(meanAnomaly);			// Mean Anomaly
    node.set_n(meanMotion);			// Mean Motion
    node.set_dn(0.0);         		// First Time Derivative of the Mean Motion
    node.set_d2n(0.0);       		// Second Time Derivative of Mean Motion 
    node.set_bstar(0.0);      		// BSTAR drag term
    node.setEphemerisType('0');		// Ephemeris type
    node.setElementNumber(0);		// Element number
    node.setRevolutionNumber(0);	// Revolution number at epoch
    
    // Output
    printf("%s \n", node.satelliteName().c_str());
//    std::cout << node.satelliteName()
//              << " [" << node.satelliteNumber()
//              << " - " << node.designator() << "]" << std::endl;
    std::cout << node;

    return 0;
}

bool useEpochForPlaneSpacing = false;

char *name;
char *noradNumber;
double baseEpoch;
double altitude;
double inclination;
double raan;
	
int parseArgs(int, char **);
void printUsage();

int main(int argc, char** argv)
{
	char buf[40];
	baseEpoch = time(NULL);
	
	if (parseArgs(argc,argv) < 0) {
		return -1;
	}
	
	double semiMajorAxis = (RADIUS_EARTH + altitude);
	double eccentricity = 0.0;
	double argumentOfPerigee = 0.0; // degrees
	double orbitalPeriod = 2 * M_PI * sqrt( pow(semiMajorAxis * 1000,3) / GM);
	double timeOfPeriapsis = 1.0;
	double meanAnomaly = 180 * (timeOfPeriapsis/orbitalPeriod); // degrees
	double timeOfPeriapsisUnit = orbitalPeriod;
	
	fprintf(stderr,"epoch %f \n", baseEpoch);
	fprintf(stderr,"semiMajorAxis %f \n", semiMajorAxis);
	fprintf(stderr,"orbitalPeriod %f (secs) %f (mins)\n", orbitalPeriod, orbitalPeriod/60.0);
	
	
			timeOfPeriapsis = timeOfPeriapsisUnit;
			// even planes, timeOfPeriapsis gets incremented by half unit
			// disabled for now
//			if (i % 2 == 1) { 
//				timeOfPeriapsis += (timeOfPeriapsisUnit / 2);
//			}
			meanAnomaly = 360 * (timeOfPeriapsis/orbitalPeriod); // degrees
			
			double epoch = baseEpoch;
			
			if (!useEpochForPlaneSpacing) {
			} else {
				epoch += (orbitalPeriod );
				timeOfPeriapsis = 0;
				meanAnomaly = 0;
			}
			
	sprintf(buf, "%s",name);		
			
	outputTle( 
		name, noradNumber, semiMajorAxis, 
		inclination, raan, eccentricity,
		argumentOfPerigee, meanAnomaly, epoch);

}

int parseArgs(int argc, char ** argv)
{
	if (argc < 6) {
		fprintf(stderr, "missing arguments \n");
		printUsage();
		return -1;
	}

	name = argv[1];
	noradNumber = argv[2];
	altitude = atof(argv[3]);
	inclination = atof(argv[4]);
	raan = atof(argv[5]);
	if (argc > 6) {
		baseEpoch = atof(argv[5]);
	}
}
void printUsage()
{
	printf("Usage: genTle name noradNumber inclination_degrees altitude_km raan_deg [--epoch secondsSinceUnixEpoch]\n");

}

