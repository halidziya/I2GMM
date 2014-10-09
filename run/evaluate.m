function [micF1,macF1,maxF,AA]=evaluate_aspire(classes,clusters,files)

uf=unique(files);
for f=1:length(uf)
curr_classes=classes(files==uf(f));
curr_clusters=clusters(files==uf(f));

ucla=unique(curr_classes);
uclu=unique(curr_clusters);
nclu=length(uclu);
ncla=length(ucla);
classes_=zeros(length(curr_classes),1);
clusters_=zeros(length(curr_clusters),1);
for i=1:ncla
    classes_(curr_classes==ucla(i))=i;
end
for i=1:nclu
    clusters_(curr_clusters==uclu(i))=i;
end



A=zeros(ncla,nclu);
for i=1:ncla
    for j=1:nclu
        A(i,j)=sum(classes_ == i & clusters_ == j);
    end
end
R=A./(sum(A,2)*ones(1,nclu));
P=A./(ones(ncla,1)*sum(A,1));

F=2*(R.*P)./(R+P);

%Flowcap I evaluation
maxF{f}=max(F,[],2);
AA{f}=A;
micF1(f)=sum((sum(A,2)/sum(sum(A))).*maxF{f});
macF1(f)=mean(maxF{f});

end


        