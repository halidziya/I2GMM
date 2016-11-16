experiments='experiments/';
folder = strcat(experiments,'parallel');
igmm_mkdir(folder);
macf1=zeros(30,1);
parfor datai=1:30
    filename = ['..\..\data\Lymph\data\' num2str(datai,'%.3d') '.csv']
    labelname = ['..\..\data\Lymph\labels\' num2str(datai,'%.3d') '.csv']
    X=dlmread(filename,',',2);
    Y=dlmread(labelname,',',2);
    prefix = char(strcat(folder,'/Lymph/'));
    mkdir([prefix,'\plots\']);
    X=igmm_normalize(X,32,false);
    
    d=size(X,2);
    k0=1;
    ki=1;
    m=d+2;
    s=150/d/log(d);
    mu0=mean(X,1);
    Psi=(m-d-1)*eye(d);%*diag([1 1 0.1 0.1 0.1]);
    alp=1; gam=1;

    fprintf(1,'Writing files...\n');
    i2gmm_createBinaryFiles(char(strcat(prefix  , num2str(datai))),X,Psi,mu0,m,k0,ki,alp,gam);

    
    data=char(strcat(prefix,num2str(datai),'.matrix'));
    prior=char(strcat(prefix,num2str(datai),'_prior.matrix'));
    params=char(strcat(prefix,num2str(datai),'_params.matrix'));
    
    %writeMat(data,X,'double');

    num_sweeps = '1000';
    burn_in='800';
    step='5';
    fprintf(1,'I2GMM is running...\n');
    cmd = ['i2gmmh.exe ',data,' ',prior,' ',params,' ',num_sweeps,' ', burn_in,' .\experiments\',num2str(datai)];
    tic;
    system(cmd);
    elapsed(datai) = toc;

    [dishes rests likelihood labels]=i2gmm_readOutput(['.\experiments\' num2str(datai)]);

    labels = align_labels(labels+1);

    %slabels=readMat(char(strcat(prefix ,num2str(datai),'.matrix.superlabels')))+1;
    %labels=readMat(char(strcat(prefix ,num2str(datai),'.matrix.labels')))+1;
    %alabels = align_labels(slabels');
    f1s=evaluationTable(Y(Y~=0),labels(Y~=0))
    macf1(datai) = table2array(f1s(1,1))
    clf;
    subplot(2,1,1);
    scatter(X(:,1),X(:,2),10,1+Y);
    colormap hsv;
    subplot(2,1,2);
    scatter(X(:,1),X(:,2),10,labels);
end