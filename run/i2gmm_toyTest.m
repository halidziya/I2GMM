clear;
%k0 = 0.1;
load('C:\Users\hzyereba\Desktop\Ide\Cspace\data\aspire_simulated_KDD14.mat')
% ki is not used but it uses same format of Aspire
%dpm_createBinaryFiles('data\toy_KDD',X,model.Psi,model.mu0,model.m,k0,0.2,0.2)

tic;
!i2gmm.exe ../data/toy.matrix ../data/toy_prior.matrix ../data/toy_params.matrix 500 100
time1=toc;
%fprintf(1,'\nReading output ...\n');
[ dishes rests likelihood1 labels]=i2gmm_readOutput();

alltables = [rests.tables];
allcust=[rests.customers];
modelabels = align_labels(labels);
[Fscore,maxF1,AA]=evaluate(Y,modelabels,ones(length(G),1));
fprintf('\n%.3f\n',maxF1{1});