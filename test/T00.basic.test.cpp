#include <map>
#include <algorithm>
#include <functional>
#include <memory>
#include <stdexcept>
#include <string>

#include <pybind11/pybind11.h>

typedef std::function< pybind11::module & (std::string const &) > ModuleGetter;



PYBIND11_MODULE(/home/djtodorovic/projects/private/AADC/BINDER/mybinder/binder/test/T00.basic.test, root_module) {
	root_module.doc() = "/home/djtodorovic/projects/private/AADC/BINDER/mybinder/binder/test/T00.basic.test module";

	std::map <std::string, pybind11::module> modules;
	ModuleGetter M = [&](std::string const &namespace_) -> pybind11::module & {
		auto it = modules.find(namespace_);
		if( it == modules.end() ) throw std::runtime_error("Attempt to access pybind11::module for namespace " + namespace_ + " before it was created!!!");
		return it->second;
	};

	modules[""] = root_module;

	static std::vector<std::string> const reserved_python_words {"nonlocal", "global", };

	auto mangle_namespace_name(
		[](std::string const &ns) -> std::string {
			if ( std::find(reserved_python_words.begin(), reserved_python_words.end(), ns) == reserved_python_words.end() ) return ns;
			else return ns+'_';
		}
	);

	std::vector< std::pair<std::string, std::string> > sub_modules {
	};
	for(auto &p : sub_modules ) modules[p.first.size() ? p.first+"::"+p.second : p.second] = modules[p.first].def_submodule( mangle_namespace_name(p.second).c_str(), ("Bindings for " + p.first + "::" + p.second + " namespace").c_str() );

	//pybind11::class_<std::shared_ptr<void>>(M(""), "_encapsulated_data_");


}

// Source list file: //home/djtodorovic/projects/private/AADC/BINDER/mybinder/binder/test/T00.basic.test.sources
// /home/djtodorovic/projects/private/AADC/BINDER/mybinder/binder/test/T00.basic.test.cpp

// Modules list file: //home/djtodorovic/projects/private/AADC/BINDER/mybinder/binder/test/T00.basic.test.modules
// 
