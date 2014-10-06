clear;
%k0 = 0.1;
load('C:\Users\hzyereba\Desktop\Ide\Cspace\data\aspire_simulated_KDD14.mat')
% ki is not used but it uses same format of Aspire
%dpm_createBinaryFiles('data\toy_KDD',X,model.Psi,model.mu0,model.m,k0,0.2,0.2)
fprintf('10 iterations to compare doubleDP and Aspire\nIt does not give output until it finishes the inference\n');
for i=1:10

tic;
!doubleDP32.exe ../../data/toy_KDD.matrix ../../data/toy_KDD_prior.matrix ../../data/toy_KDD_params.matrix 500 10 > doubleDP.txt
time1=toc;
%fprintf(1,'\nReading output ...\n');
[ dishes rests likelihood1 labels]=doubleDP_readOutput();

alltables = [rests.tables];
allcust=[rests.customers];
[Fscore,maxF1,AA]=evaluate(Y([allcust.id]),[ allcust.dishid],ones(length(G),1));

tic;
!singleAspire.exe ../../data/toy_KDD.matrix ../../data/toy_KDD_prior.matrix ../../data/toy_KDD_params.matrix 500 10 > singleAspire.txt
time2=toc;
%fprintf(1,'\nReading output ...\n');
[ dishes rests likelihood2 labels]=doubleDP_readOutput();

alltables = [rests.tables];
allcust=[rests.customers];
[Fscore,maxF2,AA]=evaluate(Y([allcust.id]),[ allcust.dishid],ones(length(G),1));

fprintf(1,'\n\nDouble DP \nF1: %.3f %.3f %.3f\nLikelihood: %.2f\nTime: %.2f',maxF1{1}(1),maxF1{1}(2),maxF1{1}(3),max(likelihood1),time1);
fprintf(1,'\n\nSingle Aspire \nF1: %.3f %.3f %.3f  \nLikelihood: %.2f\nTime: %.2f',maxF2{1}(1),maxF2{1}(2),maxF2{1}(3),max(likelihood2),time2);

end