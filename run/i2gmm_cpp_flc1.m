clear all;
clc;

currentDir=pwd;
addpath([currentDir,'\src']);
addpath([currentDir,'/../../data/FLC1/data']);
results_parentdir=[currentDir,'\experiments\FLC1\results\'];
addpath(results_parentdir);


%% load image data
load('PurdueCampusData.mat');
[coeff scores]=princomp(x);
x=scores(:,1:30);

%%
for j=1:1
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


d=size(x,2);
k0=0.05;
ki=0.5;
m=d+2;
s=150/d/log(d);
mu0=mean(x,1);
%Psi=(m-d-1)*diag(diag(cov(X)))/s;
Psi=(m-d-1)*cov(x)/s;
alp=1; gam=1;

%% ASPIRE
tic;
fprintf(1,'Writing files...\n');
doubleDP_createBinaryFiles(results_dir,x,Psi,mu0,m,k0,ki,alp,gam);
data=[results_dir,'.matrix'];
prior=[results_dir,'_prior.matrix'];
params=[results_dir,'_params.matrix'];

num_sweeps='1000';
burn_in='1';
fprintf(1,'DoubleDP running...\n');
cmd = ['doubleDP32.exe ',data,' ',prior,' ',params,' ',num_sweeps,' ', burn_in,' ',results_dir];
system(cmd);
fprintf(1,'Reading output');
elapsed_time(j)=toc;

% Write the labels
[ dishes rests likelihood]=doubleDP_readOutput(results_dir);

alltables = [rests.tables];
allcust=[rests.customers];
sortedcust([allcust.id]) = allcust;
c   = [sortedcust.dishid]';
t   = [sortedcust.tableid]';
ntables(j)=length(unique(t));
ndishes(j)=length(unique(c));

[F1,maxF,AA]=evaluate(y(y~=0),c(y~=0),ones(sum(y~=0),1));
end
