# Watson-Rover

  IOT/Rover code for the Watson Rover Project developed for the GBS Watson challenge

  More info: [video](https://vimeo.com/ibmwatson/review/130135196/ededb6b90f)

## Overview
  ![deploy](/documentation/design1.png)
  ![deploy](/documentation/design2.png)

## Components

  * RasPi: contains code hosted on Raspberry Pi board hosted on the Rover
  * Bbone:  contains the voice control code hosted on Beaglebone Black
  * Arduino:  contains arduino control code

## Getting Started

  General instructions can be found [here](https://github.com/ssudheen/Watson-Rover/raw/master/documentation/install-v1.pdf).
  In order to run the project you will have to copy the credentials to use the  Watson services.
  1. In `RasPi/VR/VR_watson_analysis_v0.3.js` update Visual Recongnition credentials.
  2. In `Bbone/S2T_Rover/voice_control.js` update the Speech-to-text service credentials.

## License

  This sample code is licensed under Apache 2.0. Full license text is available in [LICENSE](LICENSE).

## Contributing

  See [CONTRIBUTING](CONTRIBUTING.md).
