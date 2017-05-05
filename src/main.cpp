#include "mainwindow.h"
#include <QApplication>

#include "soplex.h"
/*
int main(int argc, char *argv[])
{
    QApplication a(argc, argv);
    MainWindow w;
    w.show();

    return a.exec();
}*/

#include <iostream>
#include "soplex.h"

using namespace soplex;


int main()
{
   SoPlex mysoplex;

   /* set the objective sense */
   mysoplex.setIntParam(SoPlex::OBJSENSE, SoPlex::OBJSENSE_MINIMIZE);

   /* we first add variables */
   DSVector dummycol(0);
   mysoplex.addColReal(LPCol(2.0, dummycol, infinity, 15.0));
   mysoplex.addColReal(LPCol(3.0, dummycol, infinity, 20.0));

   /* then constraints one by one */
   DSVector row1(2);
   row1.add(0, 1.0);
   row1.add(1, 5.0);
   mysoplex.addRowReal(LPRow(100.0, row1, infinity));

   /* NOTE: alternatively, we could have added the matrix nonzeros in dummycol already; nonexisting rows are then
    * automatically created. */

   /* write LP in .lp format */
   mysoplex.writeFileReal("dump.lp", NULL, NULL, NULL);

   /* solve LP */
   SPxSolver::Status stat;
   DVector prim(2);
   DVector dual(1);
   stat = mysoplex.optimize();

   /* get solution */
   if( stat == SPxSolver::OPTIMAL )
   {
      mysoplex.getPrimalReal(prim);
      mysoplex.getDualReal(dual);
      std::cout << "LP solved to optimality.\n";
      std::cout << "Objective value is " << mysoplex.objValueReal() << ".\n";
      std::cout << "Primal solution is [" << prim[0] << ", " << prim[1] << "].\n";
      std::cout << "Dual solution is [" << dual[0] << "].\n";
   }

   return 0;
}
