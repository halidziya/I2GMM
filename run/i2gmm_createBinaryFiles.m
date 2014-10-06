function dubleDP_createBinaryFiles(filename,X,Psi,mu0,m,kappa,kappai,alpha,gamma) % No labels
    mkdir('data');
    writeMat([ filename '.matrix'],X,'double');
    writeMat([ filename '_params.matrix'],[size(mu0,2) m kappa kappai alpha gamma],'double'); % Aspire format
    writeMat([ filename '_prior.matrix'],[Psi;mu0],'double');
end


function writeMat(filename,mat,prec)
[n m]=size(mat);
file=fopen(filename,'w');
fwrite(file,n,'int');
fwrite(file,m,'int');
fwrite(file,mat',prec);
fclose(file);
end