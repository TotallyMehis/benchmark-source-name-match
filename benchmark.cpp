#include <cstddef>
#include <iostream>
#include <chrono>
#include <vector>
#include <fstream>
#include <string>
#include <algorithm>

bool match_new_eq(const char* pszQuery, const char* pszNameToMatch)
{
	// If the pointers are identical, we're identical
	if (pszNameToMatch == pszQuery)
		return true;

	while (*pszNameToMatch && *pszQuery)
	{
		unsigned char cName = *pszNameToMatch;
		unsigned char cQuery = *pszQuery;
		// simple ascii case conversion
		if (cName == cQuery)
			;
		else if (cName - 'A' <= (unsigned char)'Z' - 'A' && cName - 'A' + 'a' == cQuery)
			;
		else if (cName - 'a' <= (unsigned char)'z' - 'a' && cName - 'a' + 'A' == cQuery)
			;
		else
			break;
		++pszNameToMatch;
		++pszQuery;
	}

	return (*pszQuery == 0 && *pszNameToMatch == 0);
}

bool match_new_fixed(const char *pszQuery, const char *pszNameToMatch)
{
	while (*pszNameToMatch && *pszQuery)
	{
		unsigned char cName = *pszNameToMatch;
		unsigned char cQuery = *pszQuery;
		// simple ascii case conversion
		if (cName == cQuery)
			;
		else if ((unsigned char)(cName - 'A') <= (unsigned char)'Z' - 'A' && cName - 'A' + 'a' == cQuery)
			;
		else if ((unsigned char)(cName - 'a') <= (unsigned char)'z' - 'a' && cName - 'a' + 'A' == cQuery)
			;
		else
			break;
		++pszNameToMatch;
		++pszQuery;
	}

	return (*pszQuery == 0 && *pszNameToMatch == 0);
}

bool match_new(const char *pszQuery, const char *pszNameToMatch)
{
	while (*pszNameToMatch && *pszQuery)
	{
		unsigned char cName = *pszNameToMatch;
		unsigned char cQuery = *pszQuery;
		// simple ascii case conversion
		if (cName == cQuery)
			;
		else if (cName - 'A' <= (unsigned char)'Z' - 'A' && cName - 'A' + 'a' == cQuery)
			;
		else if (cName - 'a' <= (unsigned char)'z' - 'a' && cName - 'a' + 'A' == cQuery)
			;
		else
			break;
		++pszNameToMatch;
		++pszQuery;
	}

	return (*pszQuery == 0 && *pszNameToMatch == 0);
}

bool match_old(const char *pszQuery, const char *pszNameToMatch)
{
	while (*pszNameToMatch && *pszQuery)
	{
		char cName = *pszNameToMatch;
		char cQuery = *pszQuery;
		if (cName != cQuery && tolower(cName) != tolower(cQuery)) // people almost always use lowercase, so assume that first
			break;
		++pszNameToMatch;
		++pszQuery;
	}

	return (*pszQuery == 0 && *pszNameToMatch == 0);
}

typedef bool (*t_comparison)(const char *, const char *);


static volatile bool result = false; // Trick compiler to not optimize this
std::chrono::nanoseconds benchmark(const std::vector<const char *> &names1, const std::vector<const char*> &names2, t_comparison comparison_func)
{
	auto start = std::chrono::high_resolution_clock::now();
	for (auto name1 : names1)
	{
		for (auto name2 : names2)
		{
			result = comparison_func(name1, name2);
		}
	}
	auto end = std::chrono::high_resolution_clock::now();

	return end - start;
}

long long sum(const std::vector<std::chrono::nanoseconds>& v)
{
	long long sum = 0;
	for (auto s : v) {
		sum += s.count();
	}
	return sum;
}

long long avg(const std::vector<std::chrono::nanoseconds>& v)
{
	return sum(v) / v.size();
}

long long median(const std::vector<std::chrono::nanoseconds>& sorted)
{
	return sorted[sorted.size() / 2].count();
}

long long min(const std::vector<std::chrono::nanoseconds>& sorted)
{
	return sorted[0].count();
}

long long max(const std::vector<std::chrono::nanoseconds>& sorted)
{
	return sorted[sorted.size() - 1].count();
}

bool load_names(std::vector<const char*>& entity_names)
{
	std::ifstream file("names.txt");
	if (!file.is_open())
	{
		printf("Please provide names.txt with entity names.");
		return false;
	}

	// Load the names into a single buffer.
	file.ignore(std::numeric_limits<std::streamsize>::max());
	std::streamsize size = file.gcount();
	file.seekg(0, std::ios::beg);
	char* buffer = new char[size + 1];
	file.read(buffer, size);
	buffer[size] = NULL;
	file.close();

	// Load the individual names into a vector from the buffer.
	char* c = buffer;
	while (*c != NULL)
	{
		char* p;
		p = strchr(c, '\r');
		if (p != nullptr)
		{
			*p = NULL;
			p++;
		}
		else
		{
			p = c;
		}
		p = strchr(c, '\n');
		if (p != nullptr)
		{
			*p = NULL;
			p++;
		}
		else
		{
			p = c;
		}

		if (p == c)
		{
			break;
		}

		entity_names.push_back(c);
		c = p;
	}

	return entity_names.size() > 0;
}

int main()
{
	std::vector<const char*> entity_names;
	load_names(entity_names);

	const int iterations = 1000;

	std::vector<std::chrono::nanoseconds> old_values;
	std::vector<std::chrono::nanoseconds> new_values;
	std::vector<std::chrono::nanoseconds> new_values_eq;
	std::vector<std::chrono::nanoseconds> new_values_fixed;

	for (int i = 0; i < iterations; i++) {
		std::vector<const char*> cpy(entity_names.size());
		std::copy(entity_names.begin(), entity_names.end(), cpy.begin());
		std::random_shuffle(cpy.begin(), cpy.end());

		old_values.push_back(benchmark(entity_names, cpy, match_old));
		new_values.push_back(benchmark(entity_names, cpy, match_new));
		new_values_eq.push_back(benchmark(entity_names, cpy, match_new_eq));
		new_values_fixed.push_back(benchmark(entity_names, cpy, match_new_fixed));
	}

	std::sort(old_values.begin(), old_values.end());
	std::sort(new_values.begin(), new_values.end());
	std::sort(new_values_eq.begin(), new_values_eq.end());
	std::sort(new_values_fixed.begin(), new_values_fixed.end());

	auto print_results = [&new_values](const char *name, const std::vector<std::chrono::nanoseconds>& values) {
		printf("%s%lld   %lld   %lld   %lld   %.1lf\n", name, min(values), max(values), median(values), avg(values), sum(values) / (double)sum(new_values) * 100.0);
	};
	
	printf("%i^2 string comparisons, %i iterations, in nanoseconds.\n", (int)entity_names.size(), iterations);
	printf("Name\t\t\tMin\t  Max\t    Med\t      Avg\tSum %%\n");
	print_results("C lib tolower\t\t", old_values);
	print_results("Eq check + Custom comp\t", new_values_eq);
	print_results("Custom comparison\t", new_values);
	print_results("Custom comparison fixed ", new_values_fixed);

	return 0;
}
