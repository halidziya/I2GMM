function X=igmm_normalize(X,ndims,pca)
    if (~exist('ndims','var'))        
        ndims = size(X,2);
    end
    if (~exist('pca','var'))        
        pca = true;
    end
    
    if (size(X,2)<ndims)
        ndims = size(X,2);
    end
    
    %X=(X-repmat(min(X),size(X,1),1))./repmat(max(X)-min(X),size(X,1),1);
    %X=X-ones(size(X,1),1)*mean(X,1);
    if (pca)
        cc=cov(X);
        [vv dd]=eig(cc);
        X=X*vv(:,end:-1:(end-(ndims-1))); %d=20
    end
    sg=std(X,[],1);
    X=X./(ones(size(X,1),1)*sg);
    %X=(X-repmat(min(X),size(X,1),1))./repmat(max(X)-min(X),size(X,1),1);
end