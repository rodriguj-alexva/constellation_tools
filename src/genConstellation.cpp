/*
 * generate TLE's for satellite constellation of star type.
 *
 * JMR 3/13/2020
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
	double meanAnomaly
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
    node.setPreciseEpoch(time(NULL));// epoch
    
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
    printf("%s [%s-%s]\n", node.satelliteName().c_str(), node.satelliteNumber().c_str(), node.designator().c_str());
//    std::cout << node.satelliteName()
//              << " [" << node.satelliteNumber()
//              << " - " << node.designator() << "]" << std::endl;
    std::cout << node;

    return 0;
}

char *baseName = (char *) "relay";
char *baseDesignator = (char *) "990";
int numberPlanes = 2;
int numberSatellites = 4;
double altitude = 750; // km
double inclination = 85.0; // deg
	
int parseArgs(int, char **);
void printUsage();

int main(int argc, char** argv)
{
	char buf[40];
	
	double semiMajorAxis = (RADIUS_EARTH + altitude);
	double RAAN = 0.0;
	double eccentricity = 0.0;
	double argumentOfPerigee = 0.0; // degrees
	double orbitalPeriod = 2 * M_PI * sqrt( semiMajorAxis * 1000 / GM);
	double timeOfPeriapsis = 1.0;
	double meanAnomaly = 360 * (timeOfPeriapsis/orbitalPeriod); // degrees
	
	double timeOfPeriapsisUnit = orbitalPeriod / numberSatellites;
	
	if (parseArgs(argc,argv) < 0) {
		return -1;
	}
	
	
	for (int i=0; i<numberPlanes; i++)
	{
		RAAN = (180 / numberPlanes) * i;
		
		for (int j=0; j<numberSatellites; j++)
		{
			timeOfPeriapsis = j * timeOfPeriapsisUnit;
			// even planes, timeOfPeriapsis gets incremented by half unit
			if (i % 2 == 1) { 
				timeOfPeriapsis += (timeOfPeriapsisUnit / 2);
			}
			meanAnomaly = 360 * (timeOfPeriapsis/orbitalPeriod); // degrees
			
			std::ostringstream ss;
			int indexPlane = i + 1;
			int indexSat = j + 1;
			
  			ss << baseName << "_" << indexPlane << "_" << indexSat;
			std::string name = ss.str();
			
			ss.str(""); ss.clear();
			
			int satNum = (numberPlanes * i) + j;
			sprintf(buf, "%s%02d", baseDesignator, satNum);		
			std::string designator = buf;
			
			outputTle( 
				name.c_str(), designator.c_str(), semiMajorAxis, 
				inclination, RAAN, eccentricity,
				argumentOfPerigee, meanAnomaly);
		}
	}

}

int parseArgs(int argc, char ** argv)
{
	if (argc < 4) {
		fprintf(stderr, "missing arguments \n");
		printUsage();
		return -1;
	}

	baseName = argv[1];
	numberPlanes = atoi(argv[2]);
	numberSatellites = atoi(argv[3]);
	for (int i=4; i<argc; i++)
	{
		if (!strcmp(argv[i],"--inclination"))
		{
			++i;
			if (i < argc) {
				inclination = atoi(argv[i]);
			}
		}
		else if (!strcmp(argv[i],"--altitude"))
		{
			++i;
			if (i < argc) {
				altitude = atoi(argv[i]);
			}
		}
		else if (!strcmp(argv[i],"--basedesignator"))
		{
			++i;
			if (i < argc) {
				baseDesignator = argv[i];
			}
		}
	}
}
void printUsage()
{
	printf("Usage: genConstellation baseName numberPlanes numberSatellites [--inclination value_degrees] [--altitude value_km] [--basedesignator str_3chars]\n");
}

