function panAngle = findPanAngle(A, B)
    Axy = [A(1), A(2)];
    Bxy = [B(1), B(2)];
    DP = dot(Axy, Bxy);
    invCos = DP/(norm(Axy)*norm(Bxy));
    panAngle = acosd(invCos);
    if (A(1) < B(1)) 
        panAngle = -panAngle;
    end
    
end