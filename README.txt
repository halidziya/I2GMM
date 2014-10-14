I2GMM: Infinite mixtures of Infinite Gaussian Mixtures

Description: I2GMM is a non parametric Bayesian model for complex clustering applications. It allows arbitrary number of meta-clusters with arbitrary shapes including skewed and multi-modal ones. I2GMM models each meta cluster with Dirichlet Process of Gaussian mixtures that allows these arbitrary shapes. Also meta-clusters distributed according to Dirichlet Process. Thus number meta-clusters and its components are learned from the data during the inference. We used efficient collapsed Gibbs sampler for inference using conjugacy of Gaussian distribution in hierarchy. Partial parallelization is possible by conditioning on upper layer in the hierarchy. Empirical evaluation on public datasets shows the effectiveness of the model. This model is also related with the ASPIRE model which works on grouped clustering problems. For more details about ASPIRE please refer to ASPIRE's repository. 

References : 
Halid Z. Yerebakan, Bartek Rajwa, Murat Dundar, "The Infinite Mixture of Infinite Gaussian Mixtures," to appear in NIPS'14.
Murat Dundar, Halid Z. Yerebakan, Bartek Rajwa, "Batch Discovery of Recurring Rare Classes toward Identifying Anomalous Samples," In Proceedings of the 20th Annual SIGKDD International Conference on Knowledge Discovery and Data Mining (SIGKDD'14), New York, USA, Aug 24-27 2014.

Platform: The algorithm is developed in C++11 and Visual Studio 2013. The executable is generated in Windows 7 64 bit environment, using 32-64 bit releases with compiler configuration. Multi-threading is provided by C++11 features. There is an example Matlab wrapper function (i2gmm_demo.m) in the same package. But executable is stand-alone and it can work if appropriate files are provided in the same format. 

For Linux version one can use gcc-4.9 to ensure c++11 features are supported. 

File Format:Input files are in "binary" matrix file format. In this format 2 leading integers defines the matrix size and following doubles are the matrix entries.

Example: 2 3 1.0 1.0 1.0 2.0 2.0 2.0

Installation Instructions: Copy everything in the compressed file into a new directory and add the location of the directory into the Matlab path. Run i2gmm_demo.m in the command window. If the demo runs without any problems that means the executable is compatible with your system and you are ready to use the software. If you receive an error then try to compile files. Create an empty project, add source files into your project, compile in release mode with a compiler compatible with c++11. Once you get executable, replace with the existing i2gmm.exe in the folder.

For latest version one can also use git to clone repository : git clone https://github.com/halidziya/I2GMM.git


Run: If you want to run from your IDE you should provide the path of the files as a command line argument. Command line use of the i2gmm given as follows:

i2gmm.exe datafile priorfile parametersfile [#sweeps] [#burnin] [result_dir] [sample_lag]

Matrix size for files should be nxd , nx1 , 1x6 respectively. First one is data matrix with each row is an individual data point in d dimensional space. Prior file holds the covariance matrix at top and the last row is the mean vector. Parameters are d m kappa kappa1 alpha gamma respectively in the parameter file. The last four arguments are optional and default values are 1000 100 for sweeps and burnin. Result directory specifies the output path. Default value for output path is path of executable. Sample lag defines how many gibbs iterations between two saved state after burnin. For the default value 11 post burnin samples are created. 

Limitations: Due to covariance matrix of the gaussian distributions, this algorithm does not scale well with the dimensions. Maximum dimensions used in experiments was 30. Largest size for the datasets was ~280k points. Usual processing time for 100k points is around one day. 

Source code includes 3rd party files. These files indicated with appropriate comments. munkres.m plot_gaussian_ellipsoid.m utils.cpp utils.h

Folder structure:
run > contains Matlab wrapper functions and visualizations 
src > C++ code of the i2gmm
i2gmm > Visual studio project files
data  > Flower dataset

Questions and Comments :
Please send email to hzyereba@cs.iupui.edu with title I2GMM

I2GMM is released under the terms of the GNU General Public License as published by the Free Software Foundation without any warranty.