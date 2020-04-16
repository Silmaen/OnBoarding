/**
 * \author argawaen
 * \date 08/04/2020
 */
#pragma once
#include <arduino.h>
#include "globalConfig.h"
/**
 * \brief base namespace for OnBoarding project
 */
namespace ob {
    /**
     * \brief base class for defining an manager
     *
     * this manager is setup as a template singleton
     * \tparam T template class to be setup as singleton
     */
    template<class T> class baseManager {
    public:
        /**
         * \brief getTime a reference to the status manager
         * \return instance of the status manager
         */
        static T& get(){return instance;}
        /**
         * \brief function to be called at setup time
         */
        virtual void setup() {};
         /**
         * \brief to be called every loop
         */
         virtual void frame() {};
         /**
          * \brief return the name tof the manager
          * \return the manager name
          */
         [[nodiscard]] String getName(){return managerName;}
    protected:
        static T instance; ///< instance of the Status manager
        String managerName = "UnnamedManager"; ///< the name of the manager
    private:
        /**
         * \brief private copy operator to avoid copy the singleton
         * \return Referent to this
         */
        T& operator=(const T&){}
    };

}
