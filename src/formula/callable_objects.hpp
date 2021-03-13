/*
   Copyright (C) 2014 - 2018 by David White <dave@whitevine.net>
   Part of the Battle for Wesnoth Project https://www.wesnoth.org/

   This program is free software; you can redistribute it and/or modify
   it under the terms of the GNU General Public License as published by
   the Free Software Foundation; either version 2 of the License, or
   (at your option) any later version.
   This program is distributed in the hope that it will be useful,
   but WITHOUT ANY WARRANTY.

   See the COPYING file for more details.
*/

#pragma once

#include "formula/callable.hpp"
#include "formula/formula.hpp"
#include "map/location.hpp"
#include "units/ptr.hpp"

class display_context;
class gamemap;
class team;
class terrain_type;
class unit;
class unit_type;

namespace wfl
{

class terrain_callable : public formula_callable
{
public:
	terrain_callable(const display_context& m, const map_location& loc);

	variant get_value(const std::string& key) const override;
	void get_inputs(formula_input_vector& inputs) const override;

	int do_compare(const formula_callable* callable) const override;

private:
	terrain_callable* clone() const override {return new terrain_callable(*this);}
	const map_location loc_;
	const terrain_type& t_;
	const int owner_;
};

class gamemap_callable : public formula_callable
{
public:
	explicit gamemap_callable(const display_context& g) : board_(g)
	{}

	void get_inputs(formula_input_vector& inputs) const override;
	variant get_value(const std::string& key) const override;

	const gamemap& get_gamemap() const;

private:
	gamemap_callable* clone() const override {return new gamemap_callable(*this);}
	const display_context& board_;
};

class location_callable : public formula_callable
{
public:
	explicit location_callable(const map_location& loc) : loc_(loc)
	{
		type_ = LOCATION_C;
	}

	void serialize_to_string(std::string& str) const override;

	const map_location& loc() const { return loc_; }

private:
	map_location loc_;

	variant get_value(const std::string& key) const override;

	void get_inputs(formula_input_vector& inputs) const override;
	int do_compare(const formula_callable* callable) const override;
	location_callable* clone() const override {return new location_callable(*this);}
};

class attack_type_callable : public formula_callable
{
public:
	explicit attack_type_callable(const attack_type& attack);

	variant get_value(const std::string& key) const override;
	void get_inputs(formula_input_vector& inputs) const override;

	int do_compare(const formula_callable* callable) const override;

	const attack_type& get_attack_type() const { return *att_; }

private:
	attack_type_callable* clone() const override {return new attack_type_callable(*this);}
	const_attack_ptr att_;
};

class unit_callable : public formula_callable
{
public:
	unit_callable(const map_location& loc, const unit& u) : loc_(loc), u_(u)
	{
		type_ = UNIT_C;
	}

	explicit unit_callable(const unit &u);

	variant get_value(const std::string& key) const override;
	void get_inputs(formula_input_vector& inputs) const override;

	int do_compare(const formula_callable* callable) const override;

	const unit& get_unit() const { return u_; }
	const map_location& get_location() const { return loc_; }

private:
	unit_callable* clone() const override {return new unit_callable(*this);}
	const map_location& loc_;
	const unit& u_;
};

class unit_type_callable : public formula_callable
{
public:
	explicit unit_type_callable(const unit_type& u) : u_(u)
	{
		type_ = UNIT_TYPE_C;
	}

	variant get_value(const std::string& key) const override;
	void get_inputs(formula_input_vector& inputs) const override;

	int do_compare(const formula_callable* callable) const override;

	const unit_type& get_unit_type() const { return u_; }

private:
	unit_type_callable* clone() const override {return new unit_type_callable(*this);}
	const unit_type& u_;
};

class config_callable : public formula_callable
{
public:
	explicit config_callable(const config& c) : cfg_(c) {}

	variant get_value(const std::string& key) const override;
	void get_inputs(formula_input_vector& inputs) const override;
	int do_compare(const formula_callable* callable) const override;

	const config& get_config() const { return cfg_; }

private:
	config_callable* clone() const override {return new config_callable(*this);}
	const config& cfg_;
};

class team_callable : public formula_callable
{
public:
	explicit team_callable(const team& t) : team_(t) {}

	void get_inputs(formula_input_vector& inputs) const override;
	variant get_value(const std::string& key) const override;

	const team& get_team() const { return team_; }

private:
	team_callable* clone() const override {return new team_callable(*this);}
	const team& team_;
};

class set_var_callable : public action_callable
{
public:
	set_var_callable(const std::string& key, const variant& value)
		: key_(key), value_(value)
	{}

	const std::string& key() const { return key_; }
	variant value() const { return value_; }
	variant execute_self(variant ctxt) override;

private:
	std::string key_;
	variant value_;
	variant get_value(const std::string& key) const override;

	void get_inputs(formula_input_vector& inputs) const override;
	set_var_callable* clone() const override {return new set_var_callable(*this);}
};

class safe_call_callable : public action_callable
{

public:
	safe_call_callable(const variant& main, const expression_ptr& backup)
		: main_(main)
		, backup_()
		, backup_formula_(backup)
	{}

	const variant& get_main() const { return main_; }
	const expression_ptr& get_backup() const { return backup_formula_; }

	void set_backup_result(const variant& v)
	{
		backup_ = v;
	}

	variant execute_self(variant ctxt) override;

private:
	variant main_;
	variant backup_;
	expression_ptr backup_formula_;
	variant get_value(const std::string& key) const override;

	void get_inputs(formula_input_vector& inputs) const override;
	safe_call_callable* clone() const override {return new safe_call_callable(*this);}
};

class safe_call_result : public formula_callable
{
public:
	safe_call_result(const_formula_callable_ptr callable, int status, const map_location& loc = map_location())
		: failed_callable_(callable)
		, current_unit_location_(loc)
		, status_(status)
	{}
	
	// May copy the passed callable
	safe_call_result(const formula_callable& callable, int status, const map_location& loc = map_location())
		: failed_callable_(callable.to_ptr())
		, current_unit_location_(loc)
		, status_(status)
	{}

private:
	const_formula_callable_ptr failed_callable_;
	const map_location current_unit_location_;
	const int status_;

	variant get_value(const std::string& key) const override;

	void get_inputs(formula_input_vector& inputs) const override;
	safe_call_result* clone() const override {return new safe_call_result(*this);}
};

} // namespace wfl
