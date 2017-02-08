%% Samlet effektbestemmelse
clear
clc

% hent gennemsnitsværdier for lydtryk for diffust felt samt efterklang
average;
clearvars -except gen_s gen_e
T60=gen_e;%s
p_diff_dB=gen_s;%dB

% hent farver
newcolors;

% konstanter
rho=1.2;%kg/m^3
c=343;%m/s
V=14.2*11.9*7.5;%m^3
p0=20*10^(-6);%Pa
Pa_0=10^(-12);%W (standard reference effekt for dB)

% data for målinger over flade. 1: uden ekstern kilde, 2: med ekstern kilde
Pa1_dB=load('power_lin.csv');
f_oct3=Pa1_dB(:,1);%Hz
Pa1_dB=Pa1_dB(:,2);%dB
Pa2_dB=load('power_lin2.csv');
Pa2_dB=Pa2_dB(:,2);%dB

% data for baggrundsstøj
p_background_dB=load('baggrund.csv');

% sammenling diffust lydtryk for kilde med baggrundsstøj

figure(1)
hold on
b2=bar([p_diff_dB,p_background_dB],1);
b2(1).FaceColor=colors.b;
b2(2).FaceColor=colors.o;
set(gca,'XTick',[1:3:22,24],'XTickLabel',{f_oct3(1:3:22),'Z'},'YGrid','on')
xlim([0,23])
ylim([0,70])
legend([b2(1),b2(2)],{'$\textrm{Diffust lydtryk med kilde}$','$\textrm{Baggrundsst\o j}$'},'Location','NorthWest','Interpreter','latex')
xlabel('$\textrm{Centerfrekvens for \sfrac{1}{3}-oktavfilter}\,[\textrm{Hz}]$','Interpreter','latex')
ylabel('$p\,[\textrm{dB}]$','Interpreter','latex')
cleanfigure;
hold off
matlab2tikz( '../Figurer/stoej_barplot.tikz', 'height', '\figureheight', 'width', '\figurewidth','showInfo', false )

% slet målinger der ikke kan bruges
p_diff_dB(1:4)=zeros(1,4);

% korriger de to målinger der er sammenlignelige med baggrundsstøjen (10dB)
p_diff_dB(5)=10*log10(10.^(p_diff_dB(5)/10)-10.^(p_background_dB(5)/10));
p_diff_dB(6)=10*log10(10.^(p_diff_dB(6)/10)-10.^(p_background_dB(6)/10));

% omregninger
p_diff=10.^(p_diff_dB/20)*p0;%Pa
Pa1=10.^(Pa1_dB/10)*Pa_0;%W
Pa2=10.^(Pa2_dB/10)*Pa_0;%W

% ækvivalent absorptionsareal ud fra efterklang
A=24*log(10)*V./(T60*c);

% effekt ud fra ækvivalent absorptionsareal
Pa3=A.*p_diff.^2/(4*rho*c);
Pa3_dB=10*log10(Pa3/Pa_0);

% totale effekter
Pa1_tot=sum(Pa1);
Pa2_tot=sum(Pa2);
Pa3_tot=sum(Pa3);
Pa1_tot_dB=10*log10(Pa1_tot/Pa_0);
Pa2_tot_dB=10*log10(Pa2_tot/Pa_0);
Pa3_tot_dB=10*log10(Pa3_tot/Pa_0);

% plot

figure(2)
hold on
h1=plot(f_oct3,Pa1_dB);
h2=plot(f_oct3,Pa2_dB);
h3=plot(f_oct3,Pa3_dB);
set(h1,'LineWidth',1,'LineStyle',':','Marker','x','MarkerSize',10)
set(h2,'LineWidth',1,'LineStyle',':','Marker','x','MarkerSize',10)
set(h3,'LineWidth',1,'LineStyle',':','Marker','x','MarkerSize',10)
set(gca,'XScale','log')
grid on
hold off
close(gcf);

figure(3)
hold on
b1=bar([[Pa1_dB;0;Pa1_tot_dB],[Pa2_dB;0;Pa2_tot_dB],[Pa3_dB;0;Pa3_tot_dB]],1);
b1(1).FaceColor=colors.b;
b1(2).FaceColor=colors.o;
b1(3).FaceColor=colors.y;
set(gca,'XTick',[1:3:22,24],'XTickLabel',{f_oct3(1:3:22),'Z'},'YGrid','on')
xlim([0,25])
ylim([30,100])
legend([b1(1),b1(2),b1(3)],{'$\textrm{Intensitetsmetoden}$','$\textrm{Intensitetsmetoden, med ekstern kilde}$','$\textrm{Efterklangsmetoden}$'},'Location','NorthWest','Interpreter','latex')
xlabel('$\textrm{Centerfrekvens for \sfrac{1}{3}-oktavfilter}\,[\textrm{Hz}]$','Interpreter','latex')
ylabel('$P_a\,[\textrm{dB}]$','Interpreter','latex')
cleanfigure;
hold off
matlab2tikz( '../Figurer/effekt_barplot.tikz', 'height', '\figureheight', 'width', '\figurewidth','showInfo', false )