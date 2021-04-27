/****************************************************************************
**
** Copyright (C) 2012-2021 The University of Sheffield (www.sheffield.ac.uk)
**
** This file is part of Liger.
**
** GNU Lesser General Public License Usage
** This file may be used under the terms of the GNU Lesser General
** Public License version 3 as published by the Free Software
** Foundation and appearing in the file LICENSE.LGPL included in the
** packaging of this file.  Please review the following information to
** ensure the GNU Lesser General Public License version 3 requirements
** will be met: http://www.gnu.org/licenses/lgpl-3.0.html.
**
****************************************************************************/
#include <QString>
#include <QPainterPath>
#include <iostream>
#include <math.h>

#include <libavoid/libavoid.h>

//*************************************//
using namespace Avoid;
using namespace std;

static void connCallback(void *ptr)
{
    ConnRef *connRef = (ConnRef *) ptr;

    cout<< "Connector "<<  connRef->id() << " needs rerouting!" << endl;

    const PolyLine& route = connRef->route();
    cout << "New path: ";
    for (size_t i = 0; i < route.ps.size(); ++i)
    {
        cout << ((i > 0)?"-":"") << "("
             << route.ps[i].x << "," << route.ps[i].y << ")";
    }
    cout << endl;
}

void test_polyLineRouting()
{
    cout << "Test PolyLineRouting:" << endl;
    Router *router = new Router(PolyLineRouting);

    Point srcPt(1.2, 0.5);
    Point dstPt(1.5, 4);
    ConnRef *connRef = new ConnRef(router, srcPt, dstPt);
    connRef->setCallback(connCallback, connRef);
    // Force inital callback:
    router->processTransaction();

    cout <<"\nAdding a shape." << endl;
    // Create the ShapeRef:
    Polygon shapePoly(3);
    shapePoly.ps[0] = Point(1, 1);
    shapePoly.ps[1] = Point(2.5, 1.5);
    shapePoly.ps[2] = Point(1.5, 2.5);
    ShapeRef *shapeRef = new ShapeRef(router, shapePoly);
    Q_UNUSED(shapeRef); // suppress the warning
    router->processTransaction();

    cout << "\nShifting endpoint." << endl;
    Point dstPt2(6, 4.5);
    connRef->setDestEndpoint(dstPt2);
    // It's expected you know the connector needs rerouting, so the callback
    // isn't called.  You can force it to be called though, via:
    router->processTransaction();

    delete router;
}

void test_orthogonalRouting()
{
    cout << "Test OrthogonalRouting :" << endl;
    Router *router = new Router(OrthogonalRouting);

    Point srcPt(1.2, 0.5);
    Point dstPt(1.5, 4);
    ConnRef *connRef = new ConnRef(router, srcPt, dstPt);
    connRef->setCallback(connCallback, connRef);
    // Force inital callback:
    router->processTransaction();

    cout << "\nAdding a shape." << endl;
    // Create the ShapeRef:
    Polygon shapePoly(3);
    shapePoly.ps[0] = Point(1, 1);
    shapePoly.ps[1] = Point(2.5, 1.5);
    shapePoly.ps[2] = Point(1.5, 2.5);
    ShapeRef *shapeRef = new ShapeRef(router, shapePoly);
    Q_UNUSED(shapeRef); // suppress the warning

    router->processTransaction();

    cout << "\nShifting endpoint." << endl;
    Point dstPt2(6, 4.5);
    connRef->setDestEndpoint(dstPt2);
    router->processTransaction();

    delete router;
}

/* test_differentRoutingOptions
 * The following test explores different options in libavoid and
 * demostrate how to use these options to draw feature-rich connectors.
 */
void test_differentRoutingOptions()
{
    // Paint
    QPainterPath myPath;

    // Create new router
    RouterFlag option = RouterFlag::PolyLineRouting;
    Router* router = new Router(option);

    // Create a connection ref
    // All changes should be made to this ref and then pass
    // to the router
    ConnRef* connRef = new ConnRef(router);
    connRef->setCallback(connCallback,connRef);
    QPointF srcPt(0.0,0.0);
    QPointF dstPt(2.0,2.0);

    Point src(srcPt.x(),srcPt.y());
    Point dst(dstPt.x(),dstPt.y());

    connRef->setSourceEndpoint(src);
    connRef->setDestEndpoint(dst);

    cout << "Add check points for Connref" << endl;
    QPointF p1(0.0,0.2);
    QPointF p2(1.0,0.0);
    QPointF p3(1.0,2.0);
    QPointF p4(2.0,1.8);
    QList<QPointF> checkPtList;
    checkPtList.push_back(p1);
    checkPtList.push_back(p2);
    checkPtList.push_back(p3);
    checkPtList.push_back(p4);

    vector<Avoid::Checkpoint> avoid_checkpoints(checkPtList.size());
    for (int i = 0; i < checkPtList.size(); ++i){
        avoid_checkpoints[i] = Avoid::Point(checkPtList.at(i).x(), checkPtList.at(i).y());
    }
    connRef->setRoutingCheckpoints(avoid_checkpoints);
    router->processTransaction();

    cout << "\nCurved poly lines (smooth Bezier curves)" << endl;
    Polygon oldRoute = connRef->displayRoute();
    qreal curvedAmount = 5;   // change this if you like
    Polygon curvedPolylines = oldRoute.curvedPolyline(curvedAmount);
    for(size_t i=0; i<curvedPolylines.size(); i++){
        cout << curvedPolylines.ts[i] << ": "
             <<"(" << curvedPolylines.ps[i].x << "," << curvedPolylines.ps[i].y << ")" << endl;
    }

    // generate Bezier curves
    for(size_t i=0; i<curvedPolylines.size(); i++){
        QPointF point(curvedPolylines.ps[i].x,curvedPolylines.ps[i].y);
        if(i==0){
            myPath.moveTo(point);
        } else if (curvedPolylines.ts[i]=='C'){
            QPointF c1(curvedPolylines.ps[i+1].x,curvedPolylines.ps[i+1].y);
            QPointF c2(curvedPolylines.ps[i+2].x,curvedPolylines.ps[i+2].y);
            myPath.cubicTo(point,c1,c2);
            i += 2;
        } else {
            myPath.lineTo(point);
        }
    }

    cout << "\nReroute to avoid intersections" << endl;

    cout << "\nReroute to avoid a shape (rectangle)" << endl;
    // Rectangle(center, width, height);
    Point cntr(1,1);
    qreal width = 0.5;
    qreal height = 0.8;
    Rectangle rectangle(cntr, width, height);
    ShapeRef *shapeRef = new ShapeRef(router, rectangle);
    Q_UNUSED(shapeRef); // suppress the warning

    router->processTransaction();
}

int main()
{
    cout << "Running mannual tests on libavoid" << endl;
    test_polyLineRouting();
    test_orthogonalRouting();
    test_differentRoutingOptions();
}
