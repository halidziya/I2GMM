function [Fscore,maxF,AA]=evaluate(classes,clusters,files)

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
%A
R=A./(sum(A,2)*ones(1,nclu));
P=A./(ones(ncla,1)*sum(A,1));

F=2*(R.*P)./(R+P);

%Flowcap I evaluation
maxF{f}=max(F,[],2);
AA{f}=A;
%Fscore(f)=sum((sum(A,2)/sum(sum(A))).*maxF{f});
Fscore(f)=mean(maxF{f});
% %My evaluation
% [maxF maxIn]=max(A,[],1);
% AA=zeros(ncla,ncla);
% for i=1:ncla
%     AA(:,i)=sum(A(:,maxIn==i),2);
% end
% 
% 
% R=AA./(sum(AA,2)*ones(1,ncla));
% P=AA./(ones(ncla,1)*sum(AA,1));
% 
% 
% F=2*(R.*P)./(R+P);
% F(~isfinite(F))=0;
% meanF(f)=mean(diag(F));
% keyboard;
end


        