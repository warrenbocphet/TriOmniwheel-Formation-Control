function reu=sacturation(U,Vmax,parameters,theta)

    m=parameters(1);          % the mass of the robot
    alpha=parameters(2);    % parameters of DC motor
    R=parameters(5);         % the radius of the robot
    I=parameters(6);         % the inertial of the robot
    reu=U;
    u1up=Vmax/(alpha*m)*(abs(sin(theta))+abs(sin(pi/3-theta))+abs(sin(pi/3+theta)));
    u1do=-Vmax/(alpha*m)*(abs(sin(theta))+abs(sin(pi/3-theta))+abs(sin(pi/3+theta)));
    u2up=Vmax/(alpha*m)*(abs(cos(theta))+abs(cos(pi/3-theta))+abs(cos(pi/3+theta)));
    u2do=-Vmax/(alpha*m)*(abs(cos(theta))+abs(cos(pi/3-theta))+abs(cos(pi/3+theta)));
    u3up=R/(alpha*I)*3*Vmax;
    u3do=-R/(alpha*I)*3*Vmax;
    
    if U(1)>=u1up
       reu(1)=u1up;
    elseif U(1)<=u1do
       reu(1)=u1do;
    end
    
    if U(2)>=u2up
       reu(2)=u2up;
    elseif U(2)<=u2do
       reu(2)=u2do;
    end
    
    if U(3)>=u3up
       reu(3)=u3up;
    elseif U(3)<=u3do
       reu(3)=u3do;
    end
    
end