#ifndef _UTILS_H_
#define _UTILS_H_

//
// DebuggerFugitive
//

#define BEGIN_TECHNIQUE_MAP() \
	anti_debug_ptr __technique_id_to_object(Config::ETechnique id, const std::string szName, const anti_debug_ptr &pParent) { \
		switch (id) {

#define ON_TECHNIQUE_ID(id, cls) \
		case id: \
			return anti_debug_ptr(new cls(szName, pParent));

#define END_TECHNIQUE_MAP() \
		default: \
			return nullptr; \
		} \
	}

#define REGISTER_TECHNIQUE(eId) ON_TECHNIQUE_ID(Config::ETechnique::##eId, AntiDebug_##eId)
#define CREATE_TECHNIQUE(id, name, parent) __technique_id_to_object(id,name,parent)

//
// Techniques Implementation
//

#define DECLARE_TECHNIQUE(TechniqueName) \
class AntiDebug_##TechniqueName : public Technique \
{ \
public: \
	AntiDebug_##TechniqueName(const std::string name, const std::shared_ptr<AntiDebug> &parent) \
		: Technique(name, parent) \
	{} \
	virtual bool Check() const; \
}

#define CHECK_TECHNIQUE(TechniqueName) bool AntiDebug_##TechniqueName::Check() const

#endif // _UTILS_H_
