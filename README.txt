I2GMM: Infite mixtures of Infinite Gaussian Mixtures

Description: I2GMM is non parametric bayesian model for clustering applications. It allows arbitrary number of clusters with arbitrary shapes. Two layer Dirichlet process lays behind of the algorithm. Modeling each cluster with an Dirichlet Process of Normals leads to a flexible model. Emprical evaluation on public datasets shows the effectiveness of the model. This model is also related with the ASPIRE model which works on grouped clustering problems. For more details about ASPIRE please refer to ASPIRE web site

References : 
Halid Z. Yerebakan, Bartek Rajwa, Murat Dundar, "The Infinite Mixture of Infinite Gaussian Mixtures," to appear in NIPS'14.
Murat Dundar, Halid Z. Yerebakan, Bartek Rajwa, "Batch Discovery of Recurring Rare Classes toward Identifying Anomalous Samples," In Proceedings of the 20th Annual SIGKDD International Conference on Knowledge Discovery and Data Mining (SIGKDD'14), New York, USA, Aug 24-27 2014.

Platform: The algorithm is developed in C++11. The executable is generated in Windows 7 64 bit environment, using 32-64 bit releases with compiler configuration. Multi-threading is provided by C+11 features. There is an example Matlab wrapper function (demo) in the same package. But executable is stand-alone and it can work if appropriate files provided in the same format.

File Format:Input files are in binary matrix file format. In this format 2 leading integers defines the matrix size and following doubles are the matrix entries.

Example: 2 3 1.0 1.0 1.0 2.0 2.0 2.0

Installation Instructions: Copy everything in the comprossed file into a new directory and add the location of the directory into the Matlab path. Run i2gmm_demo.m in the command window. If the demo runs without any problems that means the executable is compatible with your system and you are ready to use the software. If you receive an error then try to compile files. Create an empty project, add source files into your project, compile in release mode with a copiler compatible with c++11. Once you get executable, replace with the existing i2gmm.exe in the folder. If you want to run from your IDE you should provide the path of the files as a command line argument. Command line use of the aspire given as follows

i2gmm.exe datafile priorfile parametersfile #sweeps #burnin #threads

Matrix size for files should be nxd , nx1 , 1x6 respectively. First one is data matrix with each row is an individual data point in d dimensional space. Prior file holds the covariance matrix at top and the last row is the mean vector. Parameters are d m kappa kappai alpha gamma respectively in the parameter file. The last three arguments are optional and default values are 1000 100 and 1 respectively.

Source code includes 3rd party files. These files indicated with appropriate comments. munkres.m plot_gaussian_ellipsoid.many utils.cpp utils.h


