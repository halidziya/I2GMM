%clear all;
clc;

currentDir=pwd;
addpath([currentDir,'\src']);
addpath([currentDir,'\experiments\FC\data']);
results_parentdir=[currentDir,'\experiments\FC\results\'];
addpath(results_parentdir);

%% load image data
data_dir='..\..\data\Lymph\CSV\';
label_dir='..\..\data\Lymph\Lymph\';
list=dir([data_dir,'*.csv']);

for i=1:numel(list)
datafile=[data_dir, list(i).name];
labfile=[label_dir,list(i).name];
[num txt]=xlsread(datafile);
X=num;
[num txt]=xlsread(labfile);
Y=num;


%%
for j=1:10
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


d=size(X,2);
k0=0.05;
ki=0.5;
m=d+2;
s=150/d/log(d);
mu0=mean(X,1);
%Psi=(m-d-1)*diag(diag(cov(X)))/s;
Psi=(m-d-1)*cov(X)/s;
alp=1; gam=1;

%% IMIG

fprintf(1,'Writing files...\n');
doubleDP_createBinaryFiles(results_dir,X,Psi,mu0,m,k0,ki,alp,gam);
data=[results_dir,'.matrix'];
labels=[results_dir,'_labels.matrix'];
prior=[results_dir,'_prior.matrix'];
params=[results_dir,'_params.matrix'];


num_sweeps='1500';
burn_in='1000';
step='50';
fprintf(1,'IMIG running...\n');
cmd = ['doubleDP.exe ',data,' ',prior,' ',params,' ',num_sweeps,' ', burn_in,' ',results_dir,' ',step];
tic;
system(cmd);
elapsed_time(i,j)=toc;
fprintf(1,'Reading output');
[dishes rests likelihood labels]=doubleDP_readOutput(results_dir);
in=sum(labels<0,1);
labels(:,find(sum(in,1)==1))=[];
%allcust = [rests.customers];
%c   = [allcust.dishid]';
%labels(:,1)=c;
c = align_labels(labels);
[F1s,maxF,AA]=evaluate(Y(Y~=0),c(Y~=0),ones(sum(Y~=0),1));

%t   = [allcust.tableid]';
%ntables(i,j)=length(unique(t));
ndishes(i,j)=length(unique(c));
end
end