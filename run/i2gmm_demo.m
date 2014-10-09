clear all;
clc;

currentDir=pwd;
addpath([currentDir,'\src']);
addpath([currentDir,'\experiments\simulated\data']);
results_parentdir=[currentDir,'\experiments\simulated\results\'];
addpath(results_parentdir);


%% load image data
load('NIPS14_flower.mat');

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

%% ASPIRE

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
cmd = ['doubleDP32.exe ',data,' ',prior,' ',params,' ',num_sweeps,' ', burn_in,' ',results_dir,' ',step];
tic;
system(cmd);
elapsed_time(j)=toc;
fprintf(1,'Reading output');
[dishes rests likelihood labels]=doubleDP_readOutput_ls(results_dir);

c(:,j) = align_labels(labels);
[micF1,macF1,maxF,AA]=evaluate_aspire(Y,c(:,j),ones(length(Y),1));
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

 fname=[pwd '\experiments\simulated\figures\imig_NIPS14_flower_',num2str(j),'.ps'];
 print('-depsc','-r300',fname); 
 fixPSlinestyle(fname)
end
save('\\IN-CSCI-H40452\Users\mdundar\Desktop\MATLAB\Classifiers\ASPIRE_cpp\NIPS14\imig_flower_results_mt_ls.mat','elapsed_time','micF1Matrix','macF1Matrix','c','s','alp','gam','Psi','m','k0','ki','Psi','mu0');
clth=10;
for j=1:10
    cj=c(:,j);
    uc=unique(cj);
    nclust(j)=0;
    for t=1:length(uc)
        nc(t)=sum(cj==uc(t));
        if nc(t)>=clth
            nclust(j)=nclust(j)+1;
        end
    end
end