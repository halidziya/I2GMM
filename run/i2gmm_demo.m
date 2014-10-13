clear all;
clc;

currentDir=pwd;
addpath([currentDir,'\src']);
mkdir(currentDir,'\experiments');
mkdir(currentDir,'\experiments\simulated');
mkdir(currentDir,'\experiments\simulated\data');
mkdir(currentDir,'\experiments\simulated\results');
mkdir(currentDir,'\experiments\simulated\figures');
addpath([currentDir,'\experiments\simulated\data']);
results_parentdir=[currentDir,'\experiments\simulated\results\'];
addpath(results_parentdir);


%% load image data
load('../data/NIPS14_flower.mat');

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
Psi=(m-d-1)*diag(diag(cov(X)))/s;
alp=1; gam=1;

%% I2GMM

fprintf(1,'Writing files...\n');
i2gmm_createBinaryFiles([ results_dir 'toy' ],X,Psi,mu0,m,k0,ki,alp,gam);
data=[results_dir,'toy.matrix'];
prior=[results_dir,'toy_prior.matrix'];
params=[results_dir,'toy_params.matrix'];

num_sweeps='1500';
burn_in='1000';
step='50';
fprintf(1,'I2GMM is running...\n');
cmd = ['i2gmm.exe ',data,' ',prior,' ',params,' ',num_sweeps,' ', burn_in,' ',results_dir,' ',step];
tic;
system(cmd);
elapsed_time(j)=toc;
fprintf(1,'Reading output');
[dishes rests likelihood labels]=i2gmm_readOutput(results_dir);

c(:,j) = align_labels(labels);
[micF1,macF1,maxF,AA]=evaluate(Y,c(:,j),ones(length(Y),1));
micF1Matrix(j)=micF1
macF1Matrix(j)=macF1

uc=unique(c(:,j));
cc=hsv(length(uc));

f1=figure;
hold
for l=1:length(uc)
    plot(X(c(:,j)==uc(l),1),X(c(:,j)==uc(l),2),'k.','Marker','.','MarkerSize',4,'MarkerEdgeColor',cc(l,:))
end
axis tight

 fname=[pwd '\experiments\simulated\figures\I2GMM_NIPS14_flower_',num2str(j),'.ps'];
 print('-depsc','-r300',fname); 
 fixPSlinestyle(fname)
end