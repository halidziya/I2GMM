function best=align_labels(labels)
if ~isempty(find(labels==0,1))
    labels = labels+1; % Matlab index
end

nsample = size(labels,2);
f1agg = zeros(nsample,nsample);
for i=1:nsample
    for j=(i+1):nsample
        f1agg(i,j) = evaluate_mf1(labels(:,i),labels(:,j));
        f1agg(j,i) = f1agg(i,j);
    end
end
[a b]=sort(mean(f1agg),'descend');
nselect = ceil(nsample*0.9);
labels = labels(:,b(1:nselect));
nsample = size(labels,2);

    n = size(labels,1); % Each column is a sampled labels
    for i=1:nsample % First One is the reference
        cfm=confusionmat(labels(:,1),labels(:,i));
        cost = n - cfm;
        allignment = munkres(cost);
        [sorted inverseal]=sort(allignment);
        labels(:,i)=inverseal(labels(:,i));
    end
    best = mode(labels,2);  
end