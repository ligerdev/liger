function [ y ] = blockprint( vals,name,fileID )
nn = length(vals);
fprintf(fileID,'    %s << %.15f',name,vals(1));
for i=1:nn-1
    if(mod(i,3)==0)
        fprintf(fileID,'\n     ');
    end
    if(i==nn-1)
        fprintf(fileID,'  << %.15f;\n\n',vals(i+1));
    else
        fprintf(fileID,'  << %.15f',vals(i+1));
    end
end
end

