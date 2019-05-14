//
// Author: Ahmed Irfan <irfan@fbk.eu>
//
// This file is part of verilog2smv.
// Copyright (C) 2015 Fondazione Bruno Kessler.
//
// This program is free software: you can redistribute it and/or modify
// it under the terms of the GNU General Public License as published by
// the Free Software Foundation, either version 3 of the License, or
// (at your option) any later version.
//
// This program is distributed in the hope that it will be useful,
// but WITHOUT ANY WARRANTY; without even the implied warranty of
// MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
// GNU General Public License for more details.
//
// You should have received a copy of the GNU General Public License
// along with this program.  If not, see <http://www.gnu.org/licenses/>.
//
//

#ifndef VERILOG2SMV_YOSYS_PLUGIN_NUXMV_H
#define VERILOG2SMV_YOSYS_PLUGIN_NUXMV_H


#include "kernel/yosys.h"
#include "kernel/sigtools.h"

USING_YOSYS_NAMESPACE

struct WireInfo {
  RTLIL::IdString cell_name;
  const RTLIL::SigChunk *chunk;

  WireInfo(RTLIL::IdString c, const RTLIL::SigChunk* ch) : cell_name(c), chunk(ch) { }
};

struct WireInfoOrder {
  bool operator() (const WireInfo& x, const WireInfo& y) {
    return x.chunk < y.chunk;
  }
};

struct TransitionSystem {
  std::string modulename;
  std::vector<std::string> ivarlst;
  std::vector<std::string> varlst;
  std::vector<std::string> undrvarlst;
  std::vector<std::string> deflst;
  std::vector<std::string> initlst;
  std::vector<std::string> translst;
  std::vector<std::string> assgnlst;
  std::vector<std::string> invarlst;
  std::vector<std::string> outputlst;
};

struct Context {
  std::map<RTLIL::IdString, bool> cxt_id;
  std::map<RTLIL::SigSpec, bool> cxt_sig;

  bool get(const RTLIL::IdString id) { return cxt_id[id]; }
  bool get(const RTLIL::SigSpec sig) { return cxt_sig[sig]; }
  bool get(const RTLIL::Wire* wire) { return get(wire->name); }
  bool get(const RTLIL::Cell* cell) { return get(cell->name); }
  bool get(const RTLIL::SigChunk* chunk) {
    if (chunk->wire == NULL) { return true; }
    else if (chunk->width == chunk->wire->width && chunk->offset == 0) {
      return get(chunk->wire);
    } else { return true; }
  }

  void set(const RTLIL::IdString id, bool c) { cxt_id[id] = c; }
  void set(const RTLIL::SigSpec sig, bool c) { cxt_sig[sig] = c; }
  void set(const RTLIL::Wire* wire, bool c) { set(wire->name, c); }
  void set(const RTLIL::Cell* cell, bool c) { set(cell->name, c); }

  bool has(const RTLIL::IdString id) { 
    return (cxt_id.find(id) != cxt_id.end()); 
  }
  bool has(const RTLIL::SigSpec sig) {
    return (cxt_sig.find(sig) != cxt_sig.end());
  }
  bool has(const RTLIL::Wire* wire) { return has(wire->name); }
  bool has(const RTLIL::Cell* cell) { return has(cell->name); }
};

struct Reference {
  std::map<RTLIL::IdString, std::string> ref_id;
  std::map<RTLIL::SigSpec, std::string> ref_sig;

  std::string get(const RTLIL::IdString id) { return ref_id[id]; }
  std::string get(const RTLIL::SigSpec sig) { return ref_sig[sig]; }
  std::string get(const RTLIL::Wire* wire) { return get(wire->name); }
  std::string get(const RTLIL::Cell* cell) { return get(cell->name); }
  std::string get(const RTLIL::Memory* mem) { return get(mem->name); }

  void set(const RTLIL::IdString id, std::string s) { ref_id[id] = s; }
  void set(const RTLIL::SigSpec sig, std::string s) { ref_sig[sig] = s; }
  void set(const RTLIL::Wire* wire, std::string s) { set(wire->name, s); }
  void set(const RTLIL::Cell* cell, std::string s) { set(cell->name, s); }
  void set(const RTLIL::Memory* mem, std::string s) { set(mem->name, s); }


  bool has(const RTLIL::IdString id) {
    return (ref_id.find(id) != ref_id.end());
  }
  bool has(const RTLIL::SigSpec sig) {
    return (ref_sig.find(sig) != ref_sig.end());
  }
  bool has(const RTLIL::Wire* wire) { return has(wire->name); }
  bool has(const RTLIL::Cell* cell) { return has(cell->name); }
  bool has(const RTLIL::Memory* mem) { return has(mem->name); }
};

class TranslateModule {
 public:
    static void run(std::ostream &f, RTLIL::Module *module,
                    bool output_signal_flag) {
    TranslateModule t(f, module);
    t.output_signal = output_signal_flag;
    t.Collect();
    t.Dump();
  }

 private:
  TranslateModule(std::ostream &f, RTLIL::Module* module);
  void Collect();
  void Dump();
  void CollectIvar();
  void CollectVar();
  void CollectInitRegister();
  void CollectInitMemory();
  void CollectInit();
  void CollectMemNext(RTLIL::Memory* mem);
  void CollectRegNext();
  void CollectNext();
  void CollectTrans();
  void CollectAssgn();
  void CollectInvar();
  void CollectOutput();
  
 private:
  inline bool is_wire_bool(const RTLIL::Wire* wire) {
    return (wire->width == 1);
  }
  inline bool is_cell_register(const RTLIL::Cell* cell) {
    return (cell->type == "$dff" || cell->type == "$adff" ||
	    cell->type == "$dffsr" || cell->type == "$dlatch");
  }
  
  inline std::string mk_word1(std::string e) {
    return sav_def_expr(stringf("word1(%s)", e.c_str()));
  }
  inline std::string mk_resize(std::string e, int size) {
    log_assert(size > 0);
    return sav_def_expr(stringf("resize(%s, %d)", e.c_str(), size));
  }
  inline std::string mk_bool(std::string e) {
    return sav_def_expr(stringf("bool(%s)", e.c_str()));
  }
  inline std::string mk_new_def() {
    return stringf("__expr%zu", ts.deflst.size());
  }
  inline std::string mk_slice(std::string e, int start, int width) {
    return sav_def_expr(stringf("%s[%d:%d]", e.c_str(), start,
				start - width + 1));
  }
  inline std::string mk_concat(std::string a, const std::string b) {
    return sav_def_expr(stringf("(%s %s %s)", a.c_str(),
				cell_type_translation["$concat"].c_str(),
				b.c_str()));
  }
  inline std::string mk_signed(std::string e) {
    return sav_def_expr(stringf("signed(%s)", e.c_str()));
  }
  inline std::string mk_unsigned(std::string e) {
    return sav_def_expr(stringf("unsigned(%s)", e.c_str()));
  }
  inline std::string sav_def_expr(std::string e) {
    std::string out = mk_new_def();
    ts.deflst.push_back(stringf("%s := %s;", out.c_str(), e.c_str()));
    return out;
  }
  inline void save_wire_cxt(const RTLIL::Wire* wire) {
    if (is_wire_bool(wire)) { cxt.set(wire, false); }
    else { log_assert(wire->width > 1); cxt.set(wire, true); }
  }

  const RTLIL::SigSpec* get_cell_output(const RTLIL::Cell* cell);
  
  std::string get_mem_init_expr(RTLIL::Memory* mem, std::map<RTLIL::IdString,
				std::vector<RTLIL::Cell*>> mem_init_cells_map);
  std::string mk_compatible(std::string e, bool o_cxt, bool i_cxt, 
			    int o_width, int i_width);
  std::string get_wire_decl(const RTLIL::Wire* wire);
  std::string get_memory_decl(const RTLIL::Memory* mem);
  std::string get_wire_expr(RTLIL::Wire* wire, bool bv);
  std::string get_const_expr(const RTLIL::Const* c, int width, int offset);
  std::string get_chunk_expr(const RTLIL::SigChunk* chunk, bool bv);
  std::string get_sig_expr(const RTLIL::SigSpec* sig, bool bv);
  std::string get_sig_expr_as_bool(const RTLIL::SigSpec* sig);
  std::string get_sig_expr_as_word(const RTLIL::SigSpec* sig);
  std::string get_cell_expr(const RTLIL::Cell* cell, bool bv);

  void mk_inter_wire_map();
  void dump_ts_lst(std::vector<std::string> lst, std::string pre, std::string post);
  void setup();

 private:
  std::ostream &f;
  RTLIL::Module* module;
  TransitionSystem ts;
  Context cxt;
  Reference ref;
  std::map<std::string, std::string> cell_type_translation; 
  std::map<RTLIL::IdString, std::set<WireInfo,WireInfoOrder>> inter_wire_map;
  std::map<std::string, std::string> next;
  SigMap sigmap;
  bool output_signal;
};

class NuxmvBackend : public Backend {
public:
  NuxmvBackend() : Backend("nuxmv", "write design to nuXmv file") { }

  virtual void help() {
    log("\n");
    log("    write_nuxmv [-outputsig] [filename]\n");
    log("\n");
    log("Write the current design to an nuXmv file.\n");
    log("-outputsig : translate all output signals.\n");
  }

  virtual void execute(std::ostream *&f, std::string filename,
		       std::vector<std::string> args, RTLIL::Design *design) {
    log_header("Executing nuXmv plugin.\n");
    bool output_signal = false;
    size_t argidx=1;

    for (argidx = 1; argidx < args.size(); ++argidx)
    {
      if (args[argidx] == "-outputsig") {
        output_signal = true;
        continue;
      }
      break;
    }

    extra_args(f, filename, args, argidx);
    if (design->modules_.size() > 1) {
      log_error("Design is not flattened. Run flatten.");
    }
    auto module_it = design->modules_.begin();
    RTLIL::Module *module = module_it->second;
    TranslateModule::run(*f, module, output_signal);
  }

} NuxmvBackend;


#endif  // VERILOG2SMV_YOSYS_PLUGIN_NUXMV_H
