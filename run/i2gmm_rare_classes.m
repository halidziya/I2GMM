function [F1s likelihood exectime]=doubleDP_rare_classes(niter)
if (~exist('niter','var'))
    niter = 200;
end

currentDir=pwd;
addpath([currentDir,'\src']);
addpath([currentDir,'\experiments\rare_classes\data']);
results_parentdir=[currentDir,'\experiments\rare_classes\results\'];
addpath(results_parentdir);

load('../../data/flowcapIII_knn_subsample_th0075.mat');

done=0;
run_no=1;
while done==0
[success,message,messageid] = mkdir(results_parentdir,['run',num2str(run_no)]);
if ~isempty(message)
    run_no=run_no+1;
else
    results_dir=[results_parentdir,'run',num2str(run_no),'\'];
    done=1;
end
end

%renumber uG
uF=unique(F); % unique group ids
F_=zeros(length(F),1);
for i=1:length(uF)
    F_(F==uF(i))=i;
end
F=F_;
[sortedF sortedInd]=sort(F,'ascend');
X=X(sortedInd,:);
F=sortedF;
Y=Y(sortedInd,:);

in=F<=max(F);
d=size(X,2);
k0=0.05;
ki=0.1;
m=d+2;
s=10;
mu0=mean(X(in,:),1);
Psi=(m-d-1)*diag(diag(cov(X(in,:))))/s;
alp=1; gam=1;

%% ASPIRE




fprintf(1,'Writing files...\n');
filename  = [ results_dir 'rare_classes'];
doubleDP_createBinaryFiles( filename,X(in,:),Psi,mu0,m,k0,ki,alp,gam);

data=[filename,'.matrix'];
prior=[filename,'_prior.matrix'];
params=[filename,'_params.matrix'];
num_sweeps=num2str(niter);
burn_in='100';
fprintf(1,'doubleDP running...\n');
cmd = ['doubleDP.exe ',data,' ',prior,' ',params,' ',num_sweeps,' ', burn_in,' ',results_dir];
tic;
system(cmd);
exectime = toc;
fprintf(1,'Reading output');

% Write the labels
[ dishes rests likelihood labels]=doubleDP_readOutput(results_dir);

alltables = [rests.tables];
allcust=[rests.customers];
modelabels = align_labels(labels);
[Fscore,F1s,AA]=evaluate(Y([allcust.id]),[ allcust.dishid],ones(length(F),1));
fprintf('\n%.3f\n--->',F1s{1});
[Fscore,maxF1,AA]=evaluate(Y,modelabels,ones(length(G),1));
fprintf('\n%.3f\n',maxF1{1});
