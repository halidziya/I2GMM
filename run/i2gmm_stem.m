clear all;
clc;
currentDir=pwd;
addpath([currentDir,'\src']);
addpath([currentDir,'\experiments\FC\data']);
results_parentdir=[currentDir,'\experiments\FC\results\'];
addpath(results_parentdir);
 
 
%% load image data
load(['..\..\data\stemcell\','stemcell_all.mat']);
%load('\\IN-CSCI-H40452\Users\mdundar\Desktop\Data\FlowCapINature\FlowCAP-I\Data\FCM\csv\Lymph\CSV\stemcell_all.mat')
X_all= igmm_normalize(X_all_orig,32,false);
ug=unique(G_all);
ng=length(ug);
s_range=1:30;
macf1 = zeros(30,1);
parfor i=1:length(s_range)
in=G_all==ug(s_range(i));
X=X_all(in,:);
Y=Y_all(in);
%X=X(1:10:end,:);
%Y=Y(1:10:end);
d=size(X,2);
niter=2;
 
for t=1:niter
done=0;
run_no=1;
while done==0
[success,message,messageid] = mkdir(results_parentdir,['run',num2str(run_no)]);
if ~isempty(message)
    run_no=run_no+1;
else
    results_dir=[results_parentdir,'run',num2str(run_no),'\',num2str(i)];
    done=1;
end
end
d=size(X,2);
m=100*d+2;
k0=1;
ki=1;
%ki=10*k0;
mu0=mean(X,1);
%Psi=(m-d-1)*eye(d)*10;
%Psi=(m-d-1)*diag([1 1 0.1 0.1 1 0.1]);
Psi=(m-d-1)*diag([1 1 0.1 0.1 1 0.1]);
alp=1; gam=1;
%% IMIG
 
i2gmm_createBinaryFiles(results_dir,X,Psi,mu0,m,k0,ki,alp,gam);
data=[results_dir,'.matrix'];
labels=[results_dir,'_labels.matrix'];
prior=[results_dir,'_prior.matrix'];
params=[results_dir,'_params.matrix'];
num_sweeps='2000';
burn_in='1600';
step='10';
cmd = ['i2gmm.exe ',data,' ',prior,' ',params,' ',num_sweeps,' ', burn_in,' ',results_dir,' ',step];
tic;
system(cmd);
elapsed_time{i}(t)=toc;
[dishes rests likelihood labels]=i2gmm_readOutput(results_dir);
labels = align_labels(labels+1);
f1s = evaluationTable(Y(Y~=0),labels(Y~=0))
macf1(i) = table2array(f1s(1,1))
%t   = [allcust.tableid]';
%ntables(i,j)=length(unique(t));
%ndishes(i,j)=length(unique(c));
%micF1Matrix{i}(t)=micF1;
%macF1Matrix{i}(t)=macF1;
%f1{i}(t)=macf1(t);
maxLik{i}(t)=likelihood(end);
    clf;
    subplot(2,1,1);
    scatter(X(:,1),X(:,2),10,1+Y);
    colormap hsv;
    subplot(2,1,2);
    scatter(X(:,1),X(:,2),10,labels);
end
 
end
%for i=1:30; at(i) = mean(f1{i});end