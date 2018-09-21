// abstract factory
#include <iostream>

using namespace std;

class BenzCar
{
    private:
        std::string name_;

    public:
        void setName(std::string name)  {   name_ = name;   }
        std::string getName()           {   return this->name_; }

        virtual void drive() =  0;   
};


class BenzSportCar : public BenzCar
{
    public:
        virtual void drive()   {
            std::cout << getName() << " BenzSportCar ...\n";
        }
};

class BenzBusinessCar : public BenzCar
{
    public:
        virtual void drive()   {
            std::cout << getName() << " BenzBusinessCar ...\n";
        }
};

//==================================================================
class BmwCar
{
    private:
        std::string name_;

    public:
        void setName(std::string name)  {   name_ = name;   }
        std::string getName()           {   return this->name_; }

        virtual void drive() =  0;   
};


class BmwSportCar : public BmwCar
{
    public:
        virtual void drive()   {
            std::cout << getName() << " BmwzSportCar ...\n";
        }
};

class BmwBusinessCar : public BmwCar
{
    public:
        virtual void drive()   {
            std::cout << getName() << " BmwBusinessCar ...\n";
        }
};

//==================================================================
class AudiCar
{
    private:
        std::string name_;

    public:
        void setName(std::string name)  {   name_ = name;   }
        std::string getName()           {   return this->name_; }

        virtual void drive() =  0;   
};


class AudiSportCar : public AudiCar
{
    public:
        virtual void drive()   {
            std::cout << getName() << " AudiSportCar ...\n";
        }
};

class AudiBusinessCar : public AudiCar
{
    public:
        virtual void drive()   {
            std::cout << getName() << " AudiBusinessCar ...\n";
        }
};

//==================================================================
class Driver
{
    public:
        virtual BenzCar*    createBenzCar(std::string car) = 0;
        virtual BmwCar*     createBmwCar(std::string car) = 0;
        virtual AudiCar*    createAudiCar(std::string car) = 0;
};

class SportDriver : public Driver
{
    public:
        virtual BenzCar*    createBenzCar(std::string car)
        {
            return new BenzSportCar();
        }
        virtual BmwCar*    createBmwCar(std::string car)
        {
            return new BmwSportCar();
        }
        virtual AudiCar*    createAudiCar(std::string car)
        {
            return new AudiSportCar();
        }
};

class BusinessDriver : public Driver
{
    public:
        virtual BenzCar*    createBenzCar(std::string car)
        {
            return new BenzBusinessCar();
        }
        virtual BmwCar*    createBmwCar(std::string car)
        {
            return new BmwBusinessCar();
        }
        virtual AudiCar*    createAudiCar(std::string car)
        {
            return new AudiBusinessCar();
        }
};


int main()
{
    Driver* d = new BusinessDriver();
    AudiCar* car = d->createAudiCar("audi business");
    car->drive();
    return 1;
}
