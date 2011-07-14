
#include <iostream>
#include <fstream>
#include <boost/regex.hpp>
#include <boost/regex/icu.hpp>

#include "utf8_converter.hpp"

#include <boost/lexical_cast.hpp>
#include <vector>
#include <map>

#include "tagset.hpp"

namespace poleng
{

namespace bonsai
{
    namespace puddle
    {

Tagset::Tagset()
{
    desc_terminals = false;
    desc_separator = "#";
}

Tagset::~Tagset()
{
}

Tagset::Tagset(std::string &filename)
{
    desc_terminals = false;
    desc_separator = "#";
    readFromFile(filename);
}

void Tagset::readFromFile (std::string &filename)
{
    std::ifstream tagsetFile(filename.c_str());
    if (!tagsetFile)
    {
        std::cerr << "Could not open tagset file: " << filename << "." << std::endl;
        return;
    }

    bool attr = false;
    bool pos = false;
    bool open = false;

    boost::regex regKeyVal( "\\s*([^\\s]+)\\s*=\\s*([^\\s].*[^\\s#])?\\s*(#.*)?" );
    boost::regex regComment( "#.*" );
    boost::regex regWhite( "\\s+" );
    boost::regex regOpenClasses( "\\s*[^\\s]+(\\s\\s*[^\\s]+\\s*)*\\s*" );

    //std::vector<std::string> attrList;
    //std::map<std::string, std::vector<std::string> > attributes;
    //std::map<std::string, std::map<std::string, char> > attrMappings;
    //std::map<std::string, std::vector<std::string> > POSs;
    //std::map<std::string, char> posMappings;

    //std::map<std::string, std::string> tagset;

    char posId = 'A';

    while (!tagsetFile.eof())
    {
        std::string line;
        getline(tagsetFile, line);

        if (line == "")
            continue;
        if (boost::regex_match(line, regWhite))
            continue;
                

        if (line.find("[ATTR]") != std::string::npos)
        {
            attr = true;
            pos = false;
            open = false;
            continue;
        }

        if (line.find("[POS]") != std::string::npos)
        {
            attr = false;
            pos = true;
            open = false;
            continue;
        }

        if (line.find("[OPEN]") != std::string::npos)
        {
            attr = false;
            pos = false;
            open = true;
            continue;
        }

        if (boost::regex_match(line, regComment))
            continue;

        line = boost::regex_replace(line, regComment, "", boost::match_default | boost::format_sed);
        boost::smatch what;
        if (boost::regex_match(line, what, regKeyVal))
        {
            if (attr)
            {
                //std::cout << "Atrybut: " << what[1] << std::endl;
                std::string attrName = what[1];

                if (attributes.find(attrName) != attributes.end())
                {
                    std::cerr << "Attribute: " << attrName << " already defined. Skipping." << std::endl;
                    continue;
                }

                //std::string::iterator start, end;
                std::string values = what[2];
                boost::sregex_token_iterator i (values.begin(), values.end(), regWhite, -1);
                boost::sregex_token_iterator j;

                std::vector<std::string> attrValues;
                //std::cout << "Wartosci: " << std::endl;
                while (i != j)
                {
                    attrValues.push_back(*i);
                    //std::cout << *i << std::endl;
                    i ++;
                }

                attrList.push_back(attrName);

                attributes.insert(std::pair<std::string, std::vector<std::string> >(attrName, attrValues));

                std::vector<std::string>::iterator val;
                val = attrValues.begin();
                char mapped = 'A';
                std::map<std::string, char> mappings;
                while (val != attrValues.end())
                {
                    mappings.insert(std::pair<std::string, char>(*val, mapped));
                    mapped ++;
                    if (mapped == '[')
                        mapped = 'a';
                    if (mapped == '{')
                        mapped = '0';
                    val ++;
                }
                attrMappings.insert(std::pair<std::string, std::map<std::string, char> >(attrName, mappings));
                //std::cout << "Wartosci: " << what[2] << std::endl;
            }
            if (pos)
            {
                //std::cout << "Czesc mowy: " << what[1] << std::endl;
                //if (what[2] == "")
                //{
                //   std std::cout << "Atrybutow brak" << std::endl;
                //    continue;
                //}

                //std::string attributes = what[2];

                std::string posName = what[1];

                if (POSs.find(posName) != POSs.end())
                {
                    std::cerr << "Part of speech: " << posName << " already defined. Skipping." << std::endl;
                    continue;
                }

                std::string attrs = what[2];
                std::vector<std::string> posAttrs;

                if (attrs != "")
                {
                    boost::sregex_token_iterator i (attrs.begin(), attrs.end(), regWhite, -1);
                    boost::sregex_token_iterator j;

                    //std::cout << "Atrybuty: " << std::endl;
                    //boost::regex regOptional( "\\[([^\\s\\[\\]]+)\\]" );
                    while (i != j)
                    {
                        std::string attribute = *i;
                       //if (boost::regex_search(attribute, what, regOptional))
                       //     attribute = what[1];
                        //    attribute = "*" + what[1];

                        posAttrs.push_back(attribute);
                        //std::cout << attribute << std::endl;
                        i ++;
                    }

                }

                POSs.insert(std::pair<std::string, std::vector<std::string> >(posName, posAttrs));

                posMappings.insert(std::pair<std::string, char>(posName, posId));
                posId ++;
                if (posId == '[')
                    posId = 'a';
                if (posId == '{')
                    posId = '0';

            }
        }
        else if (boost::regex_match(line, regOpenClasses))
        {
            if (open)
            {
                boost::sregex_token_iterator i (line.begin(), line.end(), regWhite, -1);
                boost::sregex_token_iterator j;
                while (i != j)
                {
                    std::string class_name = *i;
                    std::map<std::string, char>::iterator it = posMappings.find(class_name);
                    if (it != posMappings.end())
                    {
                        open_classes.push_back(class_name);
                    }
                    else
                    {
                        std::cerr << "Unknown class: " << class_name << " given as an open class name." << std::endl;
                    }
                    i ++;
                }
            }
            else
            {
                std::cerr << "Wrong linex: " << line << std::endl;
            }
        }
        else
        {
            std::cerr << "Wrong line: " << line << std::endl;
        }

    }

    //////// generowanie start
    boost::u32regex regOptional = boost::make_u32regex( "\\[([^\\s\\[\\]]+)\\]" );
    boost::smatch wh;

    std::map<std::string, std::vector<std::string> >::iterator p;
    p = POSs.begin();
    while (p != POSs.end())
    {
        std::vector<std::string> left;
        std::vector<std::string> right;
        left.push_back(p->first);
        std::string mapped_pos = boost::lexical_cast<std::string>(posMappings[p->first]);
        std::string initRight = mapped_pos;
        int i = 0;
        while (i < attrList.size())
        {
            initRight += boost::lexical_cast<std::string>("0");
            i ++;
        }
        right.push_back(initRight);
        //right.push_back(boost::lexical_cast<std::string>(posMappings[p->first]));

      //  std::vector<std::string> attrs;
      //  attrs = p->second;
        std::vector<std::string>::iterator attrIt;
        //attrIt = attrs.begin();
        attrIt = p->second.begin();

        //while (attrIt != attrs.end())
        while (attrIt != p->second.end())  // na wszelki wypadek
        {
            bool optional = false;
            std::string attribute = *attrIt;
            if (boost::u32regex_search(attribute, wh, regOptional))
            {
                attribute = wh[1];
                optional = true;
            }
            std::vector<std::string> attrs = attributes[attribute];
            std::map<std::string, char> mappings = attrMappings[attribute];
            std::vector<std::string>::iterator val;
            std::vector<std::string>::iterator l;
            l = left.begin();
            std::vector<std::string> newLeft;
            while (l != left.end())
            {
                val = attrs.begin();
                while (val != attrs.end())
                {
                    newLeft.push_back((*l + ":" + *val));
                    val ++;
                }
                l ++;
            }
            int position = -1;
            std::vector<std::string>::iterator j;
            j = attrList.begin();
            int c = 0;
            while (j != attrList.end())
            {
                if (*j == attribute)
                {
                    position = c;
                    break;
                }
                c ++;
                j ++;
            }
            if (position == -1)
            {
                std::cerr << "Tagset error." << std::endl;
                return;
            }
            std::vector<std::string>::iterator r;
            r = right.begin();
            std::vector<std::string> newRight;
            while (r != right.end())
            {
                val = attrs.begin();
                while (val != attrs.end())
                {
                    std::string newR = *r;
                    newR[position + 1] = mappings[*val];
                    newRight.push_back(newR);
                    val ++;
                }
                r ++;
            }
            //if (optional)
            //{
            //l = left.begin();
            //while (l != left.end())
            //{
            //    newLeft.push_back
            //}
            if (!optional)
            {
                left.clear();
                left.insert(left.begin(), newLeft.begin(), newLeft.end());
                right.clear();
                right.insert(right.begin(), newRight.begin(), newRight.end());
            }
            else
            {
                left.insert(left.end(), newLeft.begin(), newLeft.end());
                right.insert(right.end(), newRight.begin(), newRight.end());
            }

            attrIt++;
        }

        std::vector<std::string>::iterator tmp;
        std::vector<std::string>::iterator tmpR;
        tmp = left.begin();
        tmpR = right.begin();
        std::vector<std::string> mapped_morphs;
        while (tmp != left.end())
        {
            morphologyMappings.insert(std::pair<std::string, std::string>(*tmp, *tmpR));
            mapped_morphs.push_back(*tmpR);
        //    std::cout << *tmp << "\t" << *tmpR << std::endl;
            tmp ++;
            tmpR ++;
        }
        mapped_morphologies_by_pos.insert(std::pair<std::string, std::vector<std::string> >(mapped_pos , mapped_morphs));
                
        p ++;
    }

    tagsetFile.close();

    //////// generowanie koniec

//    return 0;
}

std::string Tagset::log()
{
    std::stringstream ss;

    std::map<std::string, std::string>::iterator i;
    i = morphologyMappings.begin();
    while (i != morphologyMappings.end())
    {
        ss << i->first << "\t" << i->second << std::endl;
        i ++;
    }

    return ss.str();
}

bool Tagset::checkAttribute(std::string &attribute)
{
    bool ret = false;
    std::vector<std::string>::iterator i;
    i = attrList.begin();
    while (i != attrList.end())
    {
        if (*i == attribute)
        {
            ret = true;
            break;
        }
        i ++;
    }
    return ret;
}

bool Tagset::checkAttribute(std::string &pos, std::string &attribute)
{
    bool ret = false;
    std::map<std::string, std::vector<std::string> >::iterator p;
    if ((p = POSs.find(pos)) != POSs.end())
    {
        std::vector<std::string>::iterator i;
        i = p->second.begin();
        while (i != p->second.end())
        {
            if (*i == attribute)
            {
                ret = true;
                break;
            }
            if (*i == ("[" + attribute + "]"))
            {
                ret = true;
                break;
            }
            i ++;
        }
    }
    return ret;
}

bool Tagset::checkPos(std::string &pos)
{
    if (posMappings.find(pos) != posMappings.end())
        return true;
    else
        return false;
}

bool Tagset::checkAttributeValue(std::string &attribute, std::string &value)
{
    bool ret = false;
    std::string attr_tmp = attribute;
    if (attr_tmp[0] == '[' && attr_tmp[attr_tmp.size() - 1] == ']')
        attr_tmp = attr_tmp.substr(1, attr_tmp.size() - 2);
    std::map<std::string, std::vector<std::string> >::iterator p;
    if ((p = attributes.find(attr_tmp)) != attributes.end())
    {
        std::vector<std::string>::iterator i;
        i = p->second.begin();
        while (i != p->second.end())
        {
            if (*i == value)
            {
                ret = true;
                break;
            }
            i ++;
        }
    }
    return ret;
}

std::string Tagset::mapMorphology(std::string &morphology)
{
    if (morphologyMappings.find(morphology) != morphologyMappings.end())
        return morphologyMappings[morphology];
    else
        return ""; // tu by jednak trzeba zglosic blad i podniesc larum
}

std::string Tagset::unmapMorphology(std::string &mapped)
{
    for (std::map<std::string, std::string>::iterator it = morphologyMappings.begin(); it != morphologyMappings.end(); it ++)
    {
        if (it->second == mapped)
            return it->first;
    }
    return "ign";
}

char Tagset::mapPos(std::string &pos)
{
    if (posMappings.find(pos) != posMappings.end())
       return posMappings[pos];
    else
        return '\0';
}

std::pair<char, int> Tagset::mapAttributeValue(std::string &attribute, std::string &value)
{
    if (attrMappings.find(attribute) != attrMappings.end())
    {
        std::map<std::string, std::map<std::string, char> >::iterator p;
        p = attrMappings.find(attribute);
        if (p->second.find(value) != p->second.end())
        {
            char mapping = (p->second)[value];
            std::vector<std::string>::iterator i;
            int position = -1;
            i = attrList.begin();
            int c = 0;
            while (i != attrList.end())
            {
                if (*i == attribute)
                {
                    position = c;
                    break;
                }
                c ++;
                i ++;
            }
            if ((mapping != '\0') && (position > -1))
                return std::pair<char, int>(mapping, position);
        }
        //else
        //    return std::pair<char, int>('\0', -1);
    }
   // else
        return std::pair<char, int>('\0', -1);
}


int Tagset::getNumberOfAttributes()
{
    return attrList.size();
}

int Tagset::size()
{
    return posMappings.size();
}

std::vector<char> Tagset::mapPosMatching(std::string &regexp) //TODO: obsluga ladna sytuacji, ze wyrazenie regularne jest bledne (nie jest wyrazeniem regularnym)
{
    boost::regex regPos (regexp);
    std::vector<char> matching;
    std::map<std::string, char>::iterator i;
    i = posMappings.begin();
    while (i != posMappings.end())
    {
        if (boost::regex_match(i->first, regPos))
            matching.push_back(i->second);
        i ++;
    }
    return matching;
}

std::vector<char> Tagset::mapAttributeValuesMatching(std::string &attribute, std::string &regexp)
{
    boost::regex regAttr (regexp);
    std::vector<char> matching;
    if (attrMappings.find(attribute) != attrMappings.end())
    {
        std::map<std::string, std::map<std::string, char> >::iterator p;
        p = attrMappings.find(attribute);
        std::map<std::string, char>::iterator i;
        i = p->second.begin();
        while (i != p->second.end())
        {
            if (boost::regex_match(i->first, regAttr))
                matching.push_back(i->second);
            i ++;
        }
    }

    return matching;
}

int Tagset::getAttributeIndex(std::string &attribute)
{
    std::vector<std::string>::iterator i;
    i = attrList.begin();
    int index = 0;
    int ret = -1;
    while (i != attrList.end())
    {
        if (*i == attribute)
        {
            ret = index;
            break;
        }
        index ++;
        i ++;
    }
    return ret;
}

std::string Tagset::getAttributeAtIndex(std::string &pos, int index)
{
    if (POSs.find(pos) == POSs.end())
    {
        return "";
    }
    std::vector<std::string> attributes = POSs[pos];
    if (index < attributes.size())
    {
        std::string attr = attributes[index];
        if ((attr[0] == '[') && (attr[attr.length() - 1] == ']'))
            attr = attr.substr(1, attr.length() - 2);
        return attr;
    }
    else
    {
        return "";
    }

}

std::vector<std::string> Tagset::getAttributeValues(std::string &attribute)
{
    std::vector<std::string> empty;
    if (checkAttribute(attribute))
    {
        return attributes[attribute];
    }
    else
    {
        return empty;
    }
}

void Tagset::readDescFromFile(std::string &filename)
{
    std::ifstream tagsetFile(filename.c_str());
    if (!tagsetFile)
    {
        std::cerr << "Could not open description file: " << filename << "." << std::endl;
        return;
    }

    boost::regex regKeyVal( "\\s*([^\\s]+)\\s*=\\s*([^\\s].*)?\\s*(#.*)?" );
    boost::regex regComment( "#.*" );
    boost::regex regWhite( "\\s+" );

    bool general = false;
    bool posflag = false;
    bool groups = false;

    bool includepos = false;
    desc_separator = "#";

    while (!tagsetFile.eof())
    {
        std::string line;
        getline(tagsetFile, line);

        if (line == "")
            continue;
        if (boost::regex_match(line, regWhite))
            continue;
                
        if (boost::regex_match(line, regComment))
            continue;

        line = boost::regex_replace(line, regComment, "", boost::match_default | boost::format_sed);

        if (line.find("[GENERAL]") != std::string::npos)
        {
            general = true;
            posflag = false;
            groups = false;
            continue;
        }
        if (line.find("[POS]") != std::string::npos)
        {
            general = false;
            posflag = true;
            groups = false;
            continue;
        }
        if (line.find("[GROUPS]") != std::string::npos)
        {
            general = false;
            posflag = false;
            groups = true;
            continue;
        }

        if (general)
        {
            boost::smatch what;
            if (boost::regex_match(line, what, regKeyVal))
            {
                std::string key = what[1];

                if (key == "order")
                {
                    desc_order.clear();
                    std::string values = what[2];
                    boost::sregex_token_iterator i (values.begin(), values.end(), regWhite, -1);
                    boost::sregex_token_iterator j;
                    while (i != j)
                    {
                        if (*i == "orth")
                            desc_order.push_back(DESC_ORTH);
                        else if (*i == "base")
                            desc_order.push_back(DESC_BASE);
                        else if (*i == "morpho")
                            desc_order.push_back(DESC_MORPHO);
                        else
                        {
                            std::cerr << "Unknown order value: " << *i << std::endl;
                        }
                        i ++;
                    }

                    continue;
                }
                else if (key == "include-pos")
                {
                    std::string value = what[2];
                    if (value == "1")
                        includepos = true;
                    else if (value == "0")
                        includepos = false;
                    else
                        std::cerr << "Unknown value: " << value << " of option: " << key << std::endl;
                    continue;
                }
                else if (key == "desc-terminals")
                {
                    std::string value = what[2];
                    if (value == "1")
                        desc_terminals = true;
                    else if (value == "0")
                        desc_terminals = false;
                    else
                        std::cerr << "Unknown value: " << value << " of option: " << key << std::endl;
                    continue;
                }
                else if (key == "desc-nonterminals")
                {
                    std::string value = what[2];
                    if (value == "1")
                        desc_nonterminals = true;
                    else if (value == "0")
                        desc_nonterminals = false;
                    else
                        std::cerr << "Unknown value: " << value << " of option: " << key << std::endl;
                    continue;
                }
                else if (key == "separator")
                {
                    std::string value = what[2];
                    desc_separator = value;
                }
                else
                {
                    std::cerr << "Unknown option: " << key << std::endl;
                    continue;
                }

            }
        }
        desc_include_pos = includepos;

        if (posflag)
        {
            boost::smatch what;
            if (boost::regex_match(line, what, regKeyVal))
            {
                std::string pos = what[1];

                std::map<std::string, std::vector<std::string> >::iterator p = POSs.find(pos);
                if (p == POSs.end())
                {
                    std::cerr << "Unknown part of speech: " << pos << std::endl;
                    continue;
                }

                std::vector<std::string> patterns;
                std::vector<std::string> pos_vals = p->second;
                std::string values = what[2];
                boost::sregex_token_iterator i (values.begin(), values.end(), regWhite, -1);
                boost::sregex_token_iterator j;

                std::vector<int> desc;
                if (includepos)
                {
                    std::string pattern = "^(" + pos + ").*";
                    patterns.push_back(pattern);
                }
                while (i != j)
                {
                    std::map<std::string, std::vector<std::string> >::iterator q = attributes.find(*i);
                    if (q == attributes.end())
                    {
                        std::cerr << "Unknown attribute: " << *i << std::endl;
                        i ++;
                        continue;
                    }
                    std::string pattern = "";
                    for (std::vector<std::string>::iterator it = q->second.begin(); it != q->second.end(); it ++)
                    {
                        if (pattern != "")
                            pattern += "|";
                        pattern += *it;
                    }
                    pattern = ".*(" + pattern;
                    pattern += ").*";
                    patterns.push_back(pattern);
                    i ++;
                }

                token_description.insert(std::pair<std::string, std::vector<std::string> >(pos, patterns));
            }
        }

        if (groups)
        {
            boost::smatch what;
            if (boost::regex_match(line, what, regKeyVal))
            {
                std::string group = what[1];

                std::vector<std::string> patterns;
                std::string values = what[2];
                boost::sregex_token_iterator i (values.begin(), values.end(), regWhite, -1);
                boost::sregex_token_iterator j;

                while (i != j)
                {
                    std::map<std::string, std::vector<std::string> >::iterator p = attributes.find(*i);
                    if (p == attributes.end())
                    {
                        std::cerr << "Unknown attribute: " << *i << std::endl;
                        i ++;
                        continue;
                    }

                    std::string pattern = "";
                    for (std::vector<std::string>::iterator it = p->second.begin(); it != p->second.end(); it ++)
                    {
                        if (pattern != "")
                            pattern += "|";
                        pattern += *it;
                    }
                    pattern = ".*(" + pattern;
                    pattern += ").*";
                    patterns.push_back(pattern);
                    i ++;
                }

                group_description.insert(std::pair<std::string, std::vector<std::string> >(group, patterns));
            }
        }

    }
    tagsetFile.close();

}

bool Tagset::containsDesc()
{
    return (token_description.size() > 0);
}

std::vector<std::string> Tagset::getGroupDesc(std::string group)
{
    std::map<std::string, std::vector<std::string> >::iterator p = group_description.find(group);
    if (p != group_description.end())
        return p->second;
    else
    {
        std::vector<std::string> a;
        return a;
    }

}

std::vector<std::string> Tagset::getTokenDesc(std::string pos)
{
    std::map<std::string, std::vector<std::string> >::iterator p = token_description.find(pos);
    if (p != token_description.end())
        return p->second;
    else
    {
        std::vector<std::string> a;
        return a;
    }

}

std::map<std::string, std::string> Tagset::getAttributes(std::string morphologyString, bool buffer)
{
    std::map<std::string, std::string> results;
    if (this->size() == 0)
        return results;

    if (buffer)
    {
        std::map<std::string, std::map<std::string, std::string> >::iterator bufit = attributes_buffer.find(morphologyString);
        if (bufit != attributes_buffer.end())
        {
            return bufit->second;
        }
    }
    boost::regex regSep(":");
    boost::sregex_token_iterator i (morphologyString.begin(), morphologyString.end(), regSep, -1);
    boost::sregex_token_iterator j;
    std::string base = "";
    std::string pos = "";
    int attribute_start = 0;
    std::vector<std::string> attrs;
    while (i != j)
    {
        if (pos == "")
        {
            pos = *i;
            if (checkPos(pos))
            {
                results.insert(std::pair<std::string, std::string>("pos", pos));
            }
        }
        else
        {
            if (attrs.size() == 0)
            {
                std::map<std::string, std::vector<std::string> >::iterator pi = POSs.find(pos);
                attrs = pi->second;
            }
            std::vector<std::string>::iterator ai = attrs.begin() + attribute_start;
            while (ai != attrs.end())
            {
                std::string attr = *ai;
                std::string val = *i;
                if (attr[0] == '[' && attr[attr.size() - 1] == ']')
                    attr = attr.substr(1, attr.size() - 2);
                if (checkAttributeValue(attr, val))
                {
                    results.insert(std::pair<std::string, std::string>(attr, val));
                    attribute_start ++;
                    break;
                }
                else
                {
                    attribute_start ++;
                    ai ++;
                }
            }
        }
        i ++;
    }
    if (buffer)
    {
        attributes_buffer.insert(std::pair<std::string, std::map<std::string, std::string> >(morphologyString, results));
    }
    return results;
}

std::vector<std::string> Tagset::getOpenClasses()
{
    return open_classes;
}

std::vector<std::string> Tagset::getMappedMorphologies(std::string mapped_pos)
{
    std::vector<std::string> empty;
    std::map<std::string, std::vector<std::string> >::iterator it = mapped_morphologies_by_pos.find(mapped_pos);
    if (it != mapped_morphologies_by_pos.end())
    {
        return it->second;
    }
    else
    {
        return empty;
    }
}

}

}

}

//int Tagset::getNumberOfAttributes(std::string pos)
//{
//    if (POSs.find(pos) != POSs.end())
//    {
//        return (POSs.find(pos))->second.size();
//    }
//    else
//        return -1;
//}

