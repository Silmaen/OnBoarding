/**
 * \author argawaen
 * \date 09/04/2020
 */
#pragma once
#include "baseManager.h"

namespace ob::core {

    /**
     * \brief simple class to handle paths
     */
    class path{
        /**
         * \brief default constructor
         */
        path():pathName(""){}
        /**
         * \brief Destructor
         */
        ~path() =default;
        /**
         * \brief copy constructor
         */
        path(const path&) = default;
        /**
         * \brief move constructor
         */
        path(path&&) = default;
        /**
         * \brief construct with a string
         * \param path the path string
         */
        explicit path(const String& path):pathName(path) {
            if (path.endsWith("/") && path != "/")
                path.substring(0,path.length());
        }
        /**
         * \brief copy affectation
         * \return this instance
         */
        path& operator=(const path&) = default;
        /**
         * \brief move instruction
         * \return this instance
         */
        path& operator=(path&&) = default;
        /**
         * \brief append path to this one
         * \return this instance
         */
        path& operator+=(const path& p){pathName += "/" + p.pathName;return *this;}
        /**
         * \brief concatenation operator
         * \param p the second path to add
         * \return the new path
         */
        [[nodiscard]] path operator+(const path& p) const {path pp(*this);pp+=p;return pp;}
        /**
         * \brief return the file name (or dir name) of the lowest item in the path
         * \return the filename
         */
        [[nodiscard]] path filename()const {
            if (isVoid() || pathName == "/") return path();
            int idx = pathName.lastIndexOf("/");
            if (idx == -1) return *this;
            return path(pathName.substring(idx));
        }
        /**
         * \brief return the path without the last sub-folder of file
         * \return the dirname
         */
        [[nodiscard]] path dirname()const{
            if (isVoid()) return path();
            if (pathName == "/") return *this;
            int idx = pathName.lastIndexOf("/");
            if (idx == -1) return path();
            return path(pathName.substring(0,idx));

        }
        /**
         * \brief return tru if the path is void
         * \return true for void path
         */
        [[nodiscard]] bool isVoid() const{
            return pathName.length() == 0;
        }
        /**
         * \brief return true if the path is ans absolute path (relative to root)
         * \return true if the path is absolute
         */
        [[nodiscard]] bool isAbsolute() const{
            return (!isVoid()) && pathName.startsWith("/");
        }
        /**
         * \brief return true if the path is a relative one
         * \return
         */
        [[nodiscard]] bool isRelative() const{
            return ! isAbsolute();
        }
    private:
        String pathName; ///< the path name
    };


    /**
     * \brief file system manager
     */
    class fileSystem : public baseManager<fileSystem> {
        friend class baseManager<fileSystem>;
    public:

    private:
        /**
         * \brief base constructor
         */
        fileSystem();
        /**
         * \brief destructor
         */
        ~fileSystem() = default;

        /**
         * \brief private copy constructor to avoid copy
         */
        fileSystem(const fileSystem &) = default;
    };
}
