function b = readS(iStream)
 

 
% Number of messages
n = iStream.available;
 
% Buffer size = 500 characters
b = zeros(1,500);
for i = 1:n
    b(i) = iStream.read();
end

end

