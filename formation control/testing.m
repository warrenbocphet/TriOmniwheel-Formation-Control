clear,clc;
close all;

%% initial states
x1=[-20,0,0,0,pi/6,0]';x2=[-40,0,80,0,pi/3,0]';x3=[50,0,0,0,pi/2,0]';                        %triangular  x=[x,dx,y,dy,theta,d_theta]                                          
x=[x1',x2',x3']';
x1_dot=[0,0,0,0,0,0]';x2_dot=[0,0,0,0,0,0]';x3_dot=[0,0,0,0,0,0]';
x_dot=[x1_dot',x2_dot',x3_dot']';
x_now=x;
x_dotnow=x_dot;
parameters=[4.5,0.087,11.4,0.04,0.18,0.12];                                  % mass, alpha, beta, wheel radius, robot radius, inertia

%%
number_of_rover = 1;
rover = [];

for i = 1:number_of_rover
    r = rover_class;
    r.connect2rover();
    r.askID();
    rover = [rover r];
end

for i = 1:number_of_rover
    rover(i).setInitialCoor(x(6*i-5),x(6*i-3),0);
end

%% PD Controller Parameters
K=[-60,-10;-5,-1];
%% communication topology
D=2*eye(3);
Ajacency=[0,1,1;1,0,1;1,1,0];                                                 %adjacency matrix
L=D-Ajacency;                                                                  %Laplacian matrix
%% time matrix
timeframe=0.01;
totaltime=10;   
t=0:timeframe:totaltime;
%% giving reference
h1=[20*cos(t)+10; -20*sin(t); 20*sin(t)+10; 20*cos(t);t*0;t*0];
h2=[20*cos(t)+10; -20*sin(t); 20*sin(t)+100; 20*cos(t);t*0;t*0];
h3=[20*cos(t)+130; -20*sin(t); 20*sin(t)+100; 20*cos(t);t*0;t*0];
h=[h1;h2;h3];
num_agent=3;
demension=3;

error=[];
control_input=zeros(9,1);
%% loop begins
for i=1:totaltime/timeframe
    pause(0.01);
    for iter = 1:number_of_rover
        rover(iter).getCoordinate();
        x_now(6*iter-5) = rover(iter).current_coor(1);
        x_now(6*iter-3) = rover(iter).current_coor(2);
        x_now(6*iter-1) = rover(iter).current_coor(3);
        
        fprintf("At iter %i, get coordinate [%i,%i]\n", iter, rover(iter).current_coor(1), rover(iter).current_coor(2))
    end 
    
    h_now=h(:,i);
    x_dotnow=[];
    
    u_now=[];
    for j=1:num_agent
        %% parameter setting
        m=parameters(1);          % the mass of the robot
        % alpha=parameters(2);    % parameters of DC motor
        beta=parameters(3);
        % r=parameters(4);         % the radius of the wheel
        R=parameters(5);         % the radius of the robot
        I=parameters(6);         % the inertial of the robot

        theta=x_now(6*j-1);     %x=[x,x_dot,y,y_dot,theta,theta_dot]
        friction=[2;2;2];       %the friction on each wheel

        %% physical model updating
        T=[-sin(theta),-sin(pi/3-theta),sin(pi/3+theta);cos(theta),-cos(pi/3-theta),-cos(pi/3+theta);R,R,R];
        A=zeros(6,6);
        A(1:3,4:6)=eye(3);
        A(4:6,4:6)=-1*beta*diag([1/m,1/m,1/I])*(T)*(T');
        % mid=A(4:6,4:6);                       % in case for checking the combined force when the friction is piecewise
        A=[A(:,1),A(:,4),A(:,2),A(:,5),A(:,3),A(:,6)];
        A=[A(1,:);A(4,:);A(2,:);A(5,:);A(3,:);A(6,:)];
        B=kron(eye(3),[0;1]);
        C=zeros(6,3);
        C(4:6,1:3)=diag([1/m,1/m,1/I])*T;
        C=[C(1,:);C(4,:);C(2,:);C(5,:);C(3,:);C(6,:)];

        %% controller
        k1=K(1,:);
        k2=K(2,:);
        % controller of XW Dong (PD control)
        Uk=kron(eye(3),k1)*(x_now(6*j-5:6*j,1)-h_now(6*j-5:6*j,1));     % k1*(xi-hi)
       % Ud=kron(eye(3),k2)*kron(L(j,:),eye(6))*(x_now-h_now);   % k2*wij((xj-hj)-(xi-hi))
        Ud=kron(diag([1,1,0]),k2)*kron(L(j,:),eye(6))*(x_now-h_now);   % k2*wij((xj-hj)-(xi-hi))
        U=Uk+Ud;
        %% sacturation of U
        Vmax=24;             % maximum voltage of motor
        U=sacturation(U,Vmax,parameters,theta);
        
        u_now=[u_now;U];

        %% get the acceleration
        xj_dot=A*x_now(6*j-5:6*j,1)+B*U-C*friction; 
        x_dotnow=[x_dotnow;xj_dot];
    
    end
    control_input=[control_input,u_now];
    x_dot=[x_dot,x_dotnow];
    x_now=x_dotnow*timeframe+x_now;
    x_now = round(x_now);
    for iter = 1:number_of_rover
        rover(iter).sendTarget(x_now(6*iter-5),x_now(6*iter-3),0)
        fprintf("At iter %i, send target [%i,%i]\n", iter, rover(iter).target(1), rover(iter).target(2))
    end 
    fprintf("\n")
%     x=[x,x_now];
end

% error=x-h;

% 
% % figure(1)
% % subplot(2,2,1);
% % figure(1)
% % hold on;
% % axis([-2 8 -2 8]);
% % plot(x(1,:),x(3,:),'r');
% % plot(h(1,:),h(3,:),'b');
% % legend('Agent1 real','Agent1 expected');
% % xlabel('X /m');
% % ylabel('Y /m');
% % 
% % subplot(2,2,2);
% % hold on
% % axis([-2 8 -2 8]);
% % plot(x(7,:),x(9,:),'r');
% % plot(h(7,:),h(9,:),'b');
% % legend('Agent2 real','Agent2 expected');
% % xlabel('X /m');
% % ylabel('Y /m');
% % 
% % subplot(2,2,3);
% % hold on
% % axis([-2 8 -2 8]);
% % plot(x(13,:),x(15,:),'r');
% % plot(h(13,:),h(15,:),'b');
% % legend('Agent3 real','Agent3 expected');
% % xlabel('X /m');
% % ylabel('Y /m');
% % 
% % figure(2)
% % hold on
% % plot(x(1,:),x(3,:),'r');
% % plot(x(7,:),x(9,:),'b');
% % plot(x(13,:),x(15,:),'y');
% % legend('1','2','3');
% % xlabel('X /m');
% % ylabel('Y /m');
% 
% 
% figure(4)
% hold on
% plot(t,error(1,:),'r');
% plot(t,error(7,:),'y');
% plot(t,error(13,:),'b');
% legend('No.1 error','No.2 error','No.3 error');
% xlabel('Time(/s)');
% ylabel('Error of X(/m)');
% hold off
% 
% figure(5)
% hold on
% plot(t,error(3,:),'r');
% plot(t,error(9,:),'y');
% plot(t,error(15,:),'b');
% legend('No.1 error','No.2 error','No.3 error');
% xlabel('Time(/s)');
% ylabel('Error of Y(/m)');
% hold off
% 
% figure(6)
% hold on
% plot(t,error(5,:),'r');
% plot(t,error(11,:),'y');
% plot(t,error(17,:),'b');
% legend('No.1 error','No.2 error','No.3 error');
% xlabel('Time(/s)');
% ylabel('Error of Theta(/rad)');
% hold off
% 
% % figure(7)         %% dynamic motion of agents
% % for i=1:1000
% %     hold off
% %     for j=1:3
% %        draw_the_robot([x(6*(j-1)+1,i),x(6*(j-1)+3,i),x(6*(j-1)+5,i)],'r');
% %        hold on;
% %        axis([-2 8 -2 8]);
% %     end
% %     pause(0.001);
% % end
% 
% figure(8)
% hold on;
% plot(t,control_input(1,:),'r');
% plot(t,control_input(2,:),'y');
% plot(t,control_input(3,:),'b');
% legend('control_input1','control_input2','control_input3');
% xlabel('Time(/s)');
% ylabel('U(/v)');
% 
% figure(9)
% hold on;
% plot(t,control_input(4,:),'r');
% plot(t,control_input(5,:),'y');
% plot(t,control_input(6,:),'b');
% legend('control_input1','control_input2','control_input3');
% xlabel('Time(/s)');
% ylabel('U(/v)');
% 
% figure(10)
% hold on;
% plot(t,control_input(7,:),'r');
% plot(t,control_input(8,:),'y');
% plot(t,control_input(9,:),'b');
% legend('control_input1','control_input2','control_input3');
% xlabel('Time(/s)');
% ylabel('U(/v)');
