function tiltAngle = findTiltAngle(A, B)
    Ay = A(3);
    By = B(3);
    alpha = acosd(Ay/norm(A));
    beta = acosd(By/norm(B));
    tiltAngle = beta-alpha;
end
