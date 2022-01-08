//
// Created by Víctor Jiménez Rugama on 12/24/21.
//

#ifndef CYDI4_SUBVIEW_H
#define CYDI4_SUBVIEW_H
class subView {
public:
    virtual void updateView()=0;
    virtual void pressX()=0;
    virtual void pressCircle()=0;
    virtual void pressTriangle()=0;
    virtual void pressSquare()=0;
    virtual void arrowUp()=0;
    virtual void arrowDown()=0;
    virtual void arrowRight()=0;
    virtual void arrowLeft()=0;
    virtual subView * getParent()=0;
    virtual subView * getChild()=0;
    virtual void deleteChild()=0;
    virtual bool isActive()=0;
    virtual ~subView()= default;
};

#endif //CYDI4_SUBVIEW_H
