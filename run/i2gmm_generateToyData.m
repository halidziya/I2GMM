%make sure your working directory is the directory ASPIRE folder is
%located.

clc;
clear all;

%% Petals Generation
mu_p=1.0*[0 0;1 0;1/sqrt(2) 1/sqrt(2);0 1; -1/sqrt(2) 1/sqrt(2);-1 0;-1/sqrt(2) -1/sqrt(2);0 -1;1/sqrt(2) -1/sqrt(2)];
C_p(:,:,1)=0.1*eye(2); C_p(:,:,2)=0.1*eye(2); C_p(:,:,3)=0.1*eye(2); C_p(:,:,4)=0.1*eye(2); C_p(:,:,5)=0.1*eye(2); C_p(:,:,6)=0.1*eye(2); 
C_p(:,:,7)=0.1*eye(2); C_p(:,:,8)=0.1*eye(2); C_p(:,:,9)=0.1*eye(2);

X_p=[]; Y_p=[]; S_p=[]; 
n=1000;
for i=1:size(mu_p,1)
    x=mvnrnd(mu_p(i,:),C_p(:,:,i),n);
    X_p=[X_p;x;];
    S_p=[S_p;ones(n,1)*i];
end
Y_p=ones(size(X_p,1),1);


f1=figure('Color',[0.7 0.7 0.7]);
set(gca,'Color',[1 1 1]);
set(gca,'xtick',[],'ytick',[]);
hold

uS=unique(S_p);
cc=hsv(max(uS));
for k=1:length(uS)
plot_gaussian_ellipsoid(mu_p(uS(k),:), C_p(:,:,k),'-',[0 0 0],2,2);
plot(X_p(S_p==uS(k),1),X_p(S_p==uS(k),2),'k.','Marker','.','MarkerSize',4,'MarkerEdgeColor',[1 0 0]);
end

%%

%% Stem Generation

mu_s=[0 -3;0 -4;0 -5;0 -6];
C_s(:,:,1)=0.5*diag([0.01 1]); C_s(:,:,2)=0.5*diag([0.01 1]);  C_s(:,:,3)=0.5*diag([0.01 1]); C_s(:,:,4)=0.5*diag([0.01 1]); 


X_s=[]; S_s=[];
for i=1:size(mu_s,1)
    x=mvnrnd(mu_s(i,:),C_s(:,:,i),n);
    X_s=[X_s;x;];
    S_s=[S_s;ones(n,1)*(i)];
end
Y_s=[2*ones(4*n,1)];

uS=unique(S_s);
for k=1:length(uS)
plot_gaussian_ellipsoid(mu_s(uS(k),:), C_s(:,:,k),'-',[0 0 0],2,2);
plot(X_s(S_s==uS(k),1),X_s(S_s==uS(k),2),'k.','Marker','.','MarkerSize',4,'MarkerEdgeColor',[0.4 0.2 0]);
end

%% Leaf Generation

mu_l=[1 -4;2 -3;-1 -4;-2 -3];
C_l(:,:,1)=0.1*[1 -1;1 1]*[1 0;0 .5]*[1 1;-1 1]; C_l(:,:,2)=C_l(:,:,1);  
C_l(:,:,3)=0.1*[1 -1;1 1]*[.5 0;0 1]*[1 1;-1 1]; C_l(:,:,4)=C_l(:,:,3); 


X_l1=[]; S_l1=[];
for i=1:2
    x=mvnrnd(mu_l(i,:),C_l(:,:,i),n);
    X_l1=[X_l1;x;];
    S_l1=[S_l1;ones(n,1)*(i)];
end
Y_l1=[3*ones(2*n,1)];

uS=unique(S_l1);
for k=1:length(uS)
plot_gaussian_ellipsoid(mu_l(uS(k),:), C_l(:,:,k),'-',[0 0 0],2,2);
plot(X_l1(S_l1==uS(k),1),X_l1(S_l1==uS(k),2),'k.','Marker','.','MarkerSize',4,'MarkerEdgeColor',[0 1 0]);
end



X_l2=[]; S_l2=[];
for i=3:4
    x=mvnrnd(mu_l(i,:),C_l(:,:,i),n);
    X_l2=[X_l2;x;];
    S_l2=[S_l2;ones(n,1)*(i-2)];
end
Y_l2=[4*ones(2*n,1)];

uS=unique(S_l2);
for k=1:length(uS)
plot_gaussian_ellipsoid(mu_l(k+2,:), C_l(:,:,k+2),'-',[0 0 0],2,2);
plot(X_l2(S_l2==uS(k),1),X_l2(S_l2==uS(k),2),'k.','Marker','.','MarkerSize',4,'MarkerEdgeColor',[0 0 1]);
end
axis tight;
X=[X_p;X_s;X_l1;X_l2];
Y=[Y_p;Y_s;Y_l1;Y_l2];
S=[S_p;S_s;S_l1;S_l2];

fname=[pwd '\experiments\simulated\data\NIPS14_flower.mat'];
save(fname,'X','Y','S','mu_p','C_p','mu_s','C_s','mu_l','C_l');
% set(gca,'LooseInset',get(gca,'TightInset'));
% set(ha(1:nr*nc),'XTickLabel',''); set(ha,'YTickLabel','');
% cd('C:\Users\mdundar\Desktop\MATLAB\Papers\AML_FC\ToyExperiment\');
 fname=[pwd '\experiments\simulated\figures\NIPS14_flower.ps'];
 print('-depsc','-r300',fname); 
% print('-dtiff','-r300',fname);
 fixPSlinestyle(fname)

