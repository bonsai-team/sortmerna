/**
 * @file load_index.hpp
 * @brief header file for load_index.cpp
 * @parblock
 * SortMeRNA - next-generation reads filter for metatranscriptomic or total RNA
 * @copyright 2012-16 Bonsai Bioinformatics Research Group
 * @copyright 2014-16 Knight Lab, Department of Pediatrics, UCSD, La Jolla
 *
 * SortMeRNA is free software: you can redistribute it and/or modify
 * it under the terms of the GNU Lesser General Public License as published by
 * the Free Software Foundation, either version 3 of the License, or
 * (at your option) any later version.
 *
 * SortMeRNA is distributed in the hope that it will be useful,
 * but WITHOUT ANY WARRANTY; without even the implied warranty of
 * MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 * GNU Lesser General Public License for more details.
 *
 * You should have received a copy of the GNU Lesser General Public License
 * along with SortMeRNA. If not, see <http://www.gnu.org/licenses/>.
 * @endparblock
 *
 * @contributors Jenya Kopylova, jenya.kopylov@gmail.com
 *               Laurent Noé, laurent.noe@lifl.fr
 *               Pierre Pericard, pierre.pericard@lifl.fr
 *               Daniel McDonald, wasade@gmail.com
 *               Mikaël Salson, mikael.salson@lifl.fr
 *               Hélène Touzet, helene.touzet@lifl.fr
 *               Rob Knight, robknight@ucsd.edu
 */

#ifndef LOADINDEX_H
#define LOADINDEX_H


#include "indexdb.hpp"
#include "options.hpp"
#include "../alp/sls_alignment_evaluer.hpp" //! ALP program for computing the Gumbel parameters

#include <deque>



using namespace Sls;

// forward declarations
struct Readstats;
class Output;


/*! @brief Map nucleotides to integers.
Ambiguous letters map to 4.
{A/a,C/c,G/g,T/t,U/u} = {0,1,2,3,3} respectively.
*/
const char nt_table[128] = {
	4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
	4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
	4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
	4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
	4, 0, 4, 1,  4, 4, 4, 2,  4, 4, 4, 4,  4, 4, 4, 4,
	4, 4, 4, 4,  3, 3, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4,
	4, 0, 4, 1,  4, 4, 4, 2,  4, 4, 4, 4,  4, 4, 4, 4,
	4, 4, 4, 4,  3, 3, 4, 4,  4, 4, 4, 4,  4, 4, 4, 4
};

/**
 * 1. Each reference file can be indexed into multiple index parts depending on the size. 
 *    Each index file name follows a pattern <Name_Part> e.g. index1_0, index1_1 etc.
 */
struct Index {
	Runopts & opts;

	// Index
	uint16_t index_num = 0; // currrently loaded index number (DB file) Set in Main thread
	uint32_t part = 0; // currently loaded index part
	uint32_t number_elements = 0; /**< number of positions in (L+1)-mer positions table */
//	string part_str; /**< index part number [1] */
//	char* ptr_dbindex; /**< pointer to index file name [1] i.e. indexfiles[index_num].second */
	//uint32_t lnwin; /**< length of seed (sliding window L) */

	// Index stats
//	vector< pair<string, string> > myfiles; /**< vector of (FASTA file, index name) pairs for loading index */ opts
//	char** argv = 0;   /**< command line for executing SortMeRNA */
//	int argc = 0;      /**< number of arguments in command line for executing SortMeRNA */
//	bool yes_SQ = false;   /**< if true, include @SQ tags in SAM output */
	//char* acceptedstrings_sam = 0; /**< pointer to output SAM file --> Output */
	long _match = 0;    /**< Smith-Waterman score for a match */
	long _mismatch = 0; /**< Smith-Waterman score for a mismatch */
	long _gap_open = 0; /**< Smith-Waterman score for gap opening */
	long _gap_extension = 0; /**< Smith-Waterman score for gap extension */

//	vector<vector<uint32_t>> skiplengths; /**< skiplengths, three intervals at which to place seeds on read (--passes option) */ opts
	std::vector<uint16_t> num_index_parts;      /**< number of parts each index file has i.e. each index can have multiple parts. See 'load_stats' */
	std::vector<vector<index_parts_stats>> index_parts_stats_vec; /**< statistics for index files' parts */
	std::vector<uint64_t> full_ref;   /**< corrected size of each reference index (for computing E-value) */
	std::vector<uint64_t> full_read;  /**< corrected size of reads (for computing E-value) */
	std::vector<uint32_t> lnwin;      /**< length of seed (sliding window L). Unique per DB. Const. Obtained in Main thread. Thread safe */
	std::vector<uint32_t> partialwin; /**< length of seed/2 */
	std::vector<uint32_t> minimal_score; /**< minimal SW score in order to reach threshold E-value */
	//uint64_t number_total_read;      /**< total number of reads in input reads file --> ReadStats */
	std::vector<pair<double, double>> gumbel; /**< Gumbel parameters Lambda and K */
	std::vector<uint64_t> numbvs; /**< number of bitvectors at depth > 0 in [w_1] reverse or [w_2] forward */
	std::vector<uint64_t> numseq;  /**< total number of reference sequences in one complete reference database */

	std::vector<kmer> lookup_tbl; /**< reference to L/2-mer look up table */
	std::vector<kmer_origin> positions_tbl; /**< reference to (L+1)-mer positions table */

	// References
	//char* ptr_dbfile = 0; /**< pointer to reference database file  References */
	//char* buffer = 0; /**< pointer to memory slot for storing reference database References */
	//char** reference_seq = 0; /**< array of pointers to sequences in buffer References */
	//uint64_t * reference_seq_len = 0; /**< array of lengths for each sequence in buffer References */
	//uint64_t seq_part_size = 0; /**< size of memory to allocate for buffer Index::index_parts_stats_vec */
	//uint64_t numseq_part = 0; /**< number of sequences in part of database indexed  Index::index_parts_stats_vec */
	//uint64_t start_part = 0; /**< index of first sequence in current index  Index::index_parts_stats_vec */
	//bool load_for_search = 0; /**< if true, compute sequence length; if false, only load sequence References */

	Index(Runopts & opts, Readstats & readstats, Output & output)
		: 
		opts(opts), 
		num_index_parts(opts.indexfiles.size(), 0), 
		full_ref(opts.indexfiles.size(), 0),
		full_read(opts.indexfiles.size(), 0), /* ReadStats::full_read_main: total number of nucleotides in all reads <- compute_read_stats */
		lnwin(opts.indexfiles.size(), 0),
		partialwin(opts.indexfiles.size(), 0),
		minimal_score(opts.indexfiles.size(), 0),
		gumbel(opts.indexfiles.size(), std::pair<double, double>(-1.0, -1.0)),
		numbvs(opts.indexfiles.size(), 0),
		numseq(opts.indexfiles.size(), 0)
	{
		load_stats(readstats, output);
	}
	~Index() {}

	// args: index number and number of the part of the index
	void load(uint32_t idx_num, uint32_t idx_part);
	void load_stats(Readstats & readstats, Output & output); // TODO: make private?
}; // ~struct Index

class References {
public:
	References(Runopts & opts, Index & index) : opts(opts), index(index), buffer(10,0) {}
	~References() {}

	void load(uint32_t idx_num, uint32_t idx_part);
	void fix_ambiguous_char(std::string & seq);
private:
	Runopts & opts;
	Index & index;
	//uint16_t idx_num; // number of currently loaded index/reference file - Index::index_num
	//	char* ptr_dbfile; // opts
	// use vector instead of 3 variables below
	std::vector<std::string> buffer; // Container for references TODO: change name?
	//char* buffer; // holds sequences from a part of a reference file. Calculated after Index::load_stats is called.
	//char** reference_seq; // pointers to the start of every sequence in the '*buffer'
	//uint64_t* reference_seq_len; // lengths of the sequences in the '*buffer'

	//uint64_t seq_part_size; // take directly from Index::index_parts_stats_vec
	// uint64_t numseq_part; // take directly from Index::index_parts_stats_vec
	//uint64_t start_part; // take from Index::index_parts_stats_vec
	bool load_for_search;
}; // ~class References


 /*! @fn load_index_stats()
    @brief Load reference database index statistics.
    @detail
    @param vector< pair<string,string> >& myfiles
    @param char** argv
    @param int argc
    @param bool yes_SQ
    @param char* acceptedstrings_sam
    @param long _match
    @param long _mismatch
    @param long _gap_open
    @param long _gap_extension
    @param vector<vector<uint32_t> >&
    @param vector<uint16_t>& num_index_parts
    @param vector<vector<index_parts_stats> >& index_parts_stats_vec
    @param vector<uint64_t>& full_ref
    @param vector<uint64_t>& full_read
    @param vector<uint32_t>& lnwin
    @param vector<uint32_t>& partialwin
    @param vector<uint32_t>& minimal_score 
    @param uint64_t number_total_read
    @param vector<pair<double, double> >& gumbel
    @param vector<uint64_t>& numbvs 
    @param vector<uint64_t>& numseq
    @return none
*/
void
load_index_stats(
	vector< pair<string,string> >& myfiles, /**< vector of (FASTA file, index name) pairs for loading index */
    char** argv, /**< command line for executing SortMeRNA */
    int argc, /**< number of arguments in command line for executing SortMeRNA */
    bool yes_SQ, /**< if true, include @SQ tags in SAM output */
    char* acceptedstrings_sam, /**< pointer to output SAM file */
    long _match, /**< Smith-Waterman score for a match */
    long _mismatch, /**< Smith-Waterman score for a mismatch */
    long _gap_open, /**< Smith-Waterman score for gap opening */
    long _gap_extension, /**< Smith-Waterman score for gap extension */
    vector<vector<uint32_t> >& skiplengths, /**< skiplengths, three intervals at which to place seeds on read */
    vector<uint16_t>& num_index_parts, /**< number of index files */
    vector<vector<index_parts_stats> >& index_parts_stats_vec, /**< statistics for index files */
    vector<uint64_t>& full_ref, /**< corrected size of each reference index (for computing E-value) */
    vector<uint64_t>& full_read, /**< corrected size of reads (for computing E-value) */
    vector<uint32_t>& lnwin, /**< length of seed (sliding window L) */
    vector<uint32_t>& partialwin, /**< length of seed/2 */
    vector<uint32_t>& minimal_score, /**< minimal SW score in order to reach threshold E-value */
    uint64_t number_total_read, /**< total number of reads in input reads file */
    vector<pair<double, double> >& gumbel, /**< Gumbel parameters Lambda and K */
    vector<uint64_t>& numbvs, /**< number of bitvectors at depth > 0 in [w_1] reverse or [w_2] forward */
    vector<uint64_t>& numseq /**< total number of reference sequences in one complete reference database */
);

 /*! @fn load_index()
    @brief Load reference database index.
    @detail
    @param char* ptr_dbindex
    @param string part_str
    @param kmer*& lookup_tbl
    @param kmer_origin*& positions_tbl
    @param uint32_t& number_elements
    @param uint32_t lnwin
    @return none
*/
void
load_index(
	char* ptr_dbindex, /**< pointer to index file name */
    string part_str, /**< index part number */
    kmer*& lookup_tbl, /**< reference to L/2-mer look up table */
    kmer_origin*& positions_tbl, /**< reference to (L+1)-mer positions table */
    uint32_t& number_elements, /**< number of positions in (L+1)-mer positions table */
    uint32_t lnwin /**< length of seed (sliding window L) */
);

 /*! @fn load_ref()
    @brief Load reference database.
    @detail
    @param char* ptr_dbfile
    @param char* buffer
    @param char** reference_seq
    @param uint64_t* reference_seq_len
    @param uint64_t seq_part_size
    @param uint64_t numseq_part
    @param uint64_t start_part
    @param bool load_for_search
    @return none
*/
void
load_ref(
	char* ptr_dbfile, /**< pointer to reference database file */
    char* buffer, /**< pointer to memory slot for storing reference database */
    char** reference_seq, /**< array of pointers to sequences in buffer */
    uint64_t* reference_seq_len, /**< array of lengths for each sequence in buffer */
    uint64_t seq_part_size, /**< size of memory to allocate for buffer */
    uint64_t numseq_part, /**< number of sequences in part of database indexed */
    uint64_t start_part, /**< index of first sequence in current index */
    bool load_for_search /**< if true, compute sequence length; if false, only load sequence */
);

#endif