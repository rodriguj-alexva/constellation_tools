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

char *baseName = (char *) "relay";
char *baseNumber = (char *) "99";
double baseEpoch;
int numberPlanes = 2;
int numberSatellites = 4;
double altitude = 750; // km
double inclination = 85.0; // deg
	
int parseArgs(int, char **);
void printUsage();

int main(int argc, char** argv)
{
	char buf[40];
	baseEpoch = time(NULL);
	
	if (parseArgs(argc,argv) < 0) {
		return -1;
	}
	if (numberPlanes < 1) {
		fprintf(stderr,"number of planes too few \n");
		return -1;
	}
	if (numberSatellites < 1) {
		fprintf(stderr,"number of satellites too few \n");
		return -1;
	}
	
	double semiMajorAxis = (RADIUS_EARTH + altitude);
	double RAAN = 0.0;
	double eccentricity = 0.0;
	double argumentOfPerigee = 0.0; // degrees
	double orbitalPeriod = 2 * M_PI * sqrt( pow(semiMajorAxis * 1000,3) / GM);
	double timeOfPeriapsis = 1.0;
	double meanAnomaly = 180 * (timeOfPeriapsis/orbitalPeriod); // degrees
	double timeOfPeriapsisUnit = orbitalPeriod / numberSatellites;
	
	fprintf(stderr,"epoch %f \n", baseEpoch);
	fprintf(stderr,"semiMajorAxis %f \n", semiMajorAxis);
	fprintf(stderr,"orbitalPeriod %f (secs) %f (mins)\n", orbitalPeriod, orbitalPeriod/60.0);
	
	
	for (int i=0; i<numberPlanes; i++)
	{
		double RAANOffset = (180 / numberPlanes) * i;
		
		for (int j=0; j<numberSatellites; j++)
		{
			timeOfPeriapsis = j * timeOfPeriapsisUnit;
			// even planes, timeOfPeriapsis gets incremented by half unit
			// disabled for now
//			if (i % 2 == 1) { 
//				timeOfPeriapsis += (timeOfPeriapsisUnit / 2);
//			}
			meanAnomaly = 360 * (timeOfPeriapsis/orbitalPeriod); // degrees
			
//			RAAN = (45 / numberSatellites) * j * -1;
			double RAANFactor;
//			RAANFactor = 25.5; // for alt 750 km
//			RAANFactor = 28.4; // for alt 1500 km
			RAANFactor = ((29.0 * altitude) / 7500.0) + (113.0/5.0);
			double epoch = baseEpoch;
			
			if (!useEpochForPlaneSpacing) {
				RAAN = RAANOffset + 360 - ((RAANFactor / numberSatellites) * j);
			} else {
				RAAN = RAANOffset;
				epoch += ((orbitalPeriod / numberSatellites) * j);
				timeOfPeriapsis = 0;
				meanAnomaly = 0;
			}
			
			std::ostringstream ss;
			int indexPlane = i + 1;
			int indexSat = j + 1;
			
  			ss << baseName << "_" << indexPlane << "_" << indexSat;
			std::string name = ss.str();
			
			ss.str(""); ss.clear();
			
			int satNum = (numberSatellites * i) + j;
			sprintf(buf, "%s%03d", baseNumber, satNum);		
			std::string designator = buf;
			
			outputTle( 
				name.c_str(), designator.c_str(), semiMajorAxis, 
				inclination, RAAN, eccentricity,
				argumentOfPerigee, meanAnomaly, epoch);
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
	baseNumber = argv[2];
	numberPlanes = atoi(argv[3]);
	numberSatellites = atoi(argv[4]);
	for (int i=5; i<argc; i++)
	{
		if (!strcmp(argv[i],"-e")) {
			useEpochForPlaneSpacing = true;
		}
		else if (!strcmp(argv[i],"--inclination"))
		{
			++i;
			if (i < argc) {
				inclination = atoi(argv[i]);
			} else {
				fprintf(stderr,"--inclination missing argument \n");
			}
		}
		else if (!strcmp(argv[i],"--altitude"))
		{
			++i;
			if (i < argc) {
				altitude = atof(argv[i]);
				
				fprintf(stderr,"altitude %f \n", altitude);
			} else {
				fprintf(stderr,"--altitude missing argument \n");
			}
		}
		else if (!strcmp(argv[i],"--epoch"))
		{
			++i;
			if (i < argc) {
				baseEpoch = atof(argv[i]);
			} else {
				fprintf(stderr,"--epoch missing argument \n");
			}
		}
		else
		{
			fprintf(stderr,"unknown parameter %s \n", argv[i]);
		}
	}
}
void printUsage()
{
	printf("Usage: genConstellation baseName baseNumber numberPlanes numberSatellites [--inclination value_degrees] [--altitude value_km] [--epoch secondsSinceUnixEpoch]\n");
//	printf("\t-e : use the Epoch value to space satellites in each plane (experimental) \n");
}

