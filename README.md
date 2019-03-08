# theta-stim
BU Senior Design Project 2018-19: The Effect of Phase-Specific Optogenetic Stimulation on Memory Recall in Mice

Theta waves (slow sinusoidal oscillations) are hallmarks of memory encoding and retrieval in the hippocampus. A prominent theoretical model proposes that different phases of theta waves separates encoding of new information from recalling stored memories. The RTXI (Real Time eXperimental Interface, http://rtxi.org/) modules in this repository aid understanding the functional role of theta in real-time.

ptDetector: Identifies and stimulates on peaks or troughs (depending on user input) of a theta wave.
ptDetector_predict: Simulates on predicted location of peaks or troughs (depending on user input) of a theta wave given a fitler time delay.
thetaFilter: Filters the LFP (local field potential) from 4-10 Hz using an IIR Chebyshev Type II filter.
LFPRead: Reads in sample LFP data to simulate the *in vivo* experimental enviornment.

