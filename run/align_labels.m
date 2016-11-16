function best=align_labels(labels)
if ~isempty(find(labels==0,1))
    labels = labels+1; % Matlab index
end

    n = size(labels,1); % Each column is a sampled labels
    nsample = size(labels,2)-1;
    for i=1:nsample % First One is the reference
        cfm=confusionmat(labels(:,end),labels(:,i));
        cost = n - cfm;
        allignment = munkres(cost);
        [sorted inverseal]=sort(allignment);
        labels(:,i)=inverseal(labels(:,i));
    end
    best = mode(labels,2);  
end