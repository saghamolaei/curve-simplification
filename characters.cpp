#include <iostream>
#include <vector>
#include <algorithm>
#include <iomanip>
using namespace std;

#define double float
#define point pair<double,double>
#define N 200
#define r 0.05
#define eps 0.0001
#define rounder 0.001

double dist(const point &a, const point &b)
{
	return sqrt((a.first - b.first)*(a.first - b.first) + (a.second - b.second)*(a.second - b.second));
}

point minuss(point a, const point &b)
{
	a.first = a.first - b.first;
	a.second = a.second - b.second;
	return a;
}

point origin = make_pair(0, 0);
double cross(const point &a, const point &b)
{
	return a.first*b.second - a.second*b.first;
}
point perp(const point &a, const point &b, const double &h)
{
	point q;
	q.first = b.first - a.first;
	q.second = b.second - a.second;
	q.first = q.first / dist(b, a);
	q.second = q.second / dist(b, a);
	double d = sqrt(dist(a, origin)*dist(a, origin) - h*h);
	q.first = a.first + q.first*d;
	q.second = a.second + q.second*d;
	return q;
}
//is q on ab? (assuming q is on the line containing ab)
bool on(const point &q, const point &a, const point &b)
{
	// a=b
	if (fabs(a.first - b.first) < eps && fabs(a.second - b.second) < eps)
	{
		if (fabs(a.first - q.first) < eps && fabs(a.second - q.second) < eps)
			return true;
		return false;
	}
	// a!=b
	if (dist(a, q) < dist(a, b) + eps && dist(b, q) < dist(a, b) + eps)
		return true;
	return false;
}

vector<point> intersect(point a, point b, const point &p)
{
	vector<point> solution;
	//outside the circle
	if (dist(a, p) + eps > r && dist(b, p) + eps > r)
		return solution;
	//inside the circle
	if (dist(a, p) < r + eps && dist(b, p) < r + eps)
	{
		// endpoint intersections do not matter
		return solution;
	}
	// a=b
	if (dist(a, b) < eps)
	{
		// if there is an intersection, it lies on a or b
		return solution;
	}
	//intersecting the circle: 1 point (tangent or half-segment): on a or on b; or 2 points
	// translation: circle center to origin
	a = minuss(a, p);
	b = minuss(b, p);
	// compute the base of the perpendicular from p to ab
	double h = cross(a, b) / (dist(a, b));
	point q = perp(a, b, h);
	//is it tangent at q
	if (on(q, a, b) && fabs(dist(q, origin) - r) < eps)
	{
		q.first += p.first;
		q.second += p.second;
		solution.push_back(q);
		return solution;
	}
	double d = sqrt(r*r - h*h);
	//direction vector from a to b
	point i;
	i.first = (b.first - a.first) / dist(a, b);
	i.second = (b.second - a.second) / dist(a, b);
	solution.push_back(q);
	solution[0].first += d*i.first;
	solution[0].second += d*i.second;
	if (on(solution[0], a, b) == false)
		solution.pop_back();
	solution.push_back(q);
	solution.back().first -= d*i.first;
	solution.back().second -= d*i.second;
	if (on(solution.back(), a, b) == false)
		solution.pop_back();
	// translation: circle center back from the origin
	for (int j = 0; j < solution.size(); j++)
	{
		solution[j].first += p.first;
		solution[j].second += p.second;
	}
	return solution;
}

bool increasing(vector<point> &a)
{
	if (a[0].first < a[1].first + eps || (fabs(a[0].first - a[1].first) < eps && a[0].second < a[1].second))
		return true;
	return false;
}
bool comp(const point &a, const point &b)
{
	if (fabs(a.first - b.first) < eps && fabs(a.second - b.second) < eps)
		return true;
	return false;
}
void adds(const vector<point> &a, const vector<point> &b, vector<point> &output)
{
	vector<point> news;
	vector<point> temp;
	for (int k = 0; k < a.size(); k++)
	{
		news.push_back(a[k]);
		if (k + 1 < a.size())
		{
			vector<point> addit;
			for (int i = 0; i < b.size(); i++)
			{
				temp.erase(temp.begin(), temp.end());
				temp = intersect(a[k], a[k + 1], b[i]);
				for (int j = 0; j < temp.size(); j++)
					addit.push_back(temp[j]);
			}
			temp.erase(temp.begin(), temp.end());
			temp.push_back(a[k]);
			temp.push_back(a[k + 1]);
			if (increasing(temp))
			{
				sort(addit.begin(), addit.end());
			}
			else
			{
				sort(addit.rbegin(), addit.rend());
			}
			for (int i = 0; i < addit.size(); i++)
				news.push_back(addit[i]);
		}
	}
	output.assign(news.begin(), news.end());
	output.resize(unique(output.begin(), output.end(), comp) - output.begin());
}

bool fresh(vector<point> ai, vector<point> bi)
{
	//add points of the second curve to the first one
	vector<point> a, b;
	adds(ai, bi, a);
	adds(bi, ai, b);
	//compute the discrete frechet distance
	bool FD[N][N];
	pair<int, int> par[N][N];
	for (int i = 0; i < a.size(); i++)
	for (int j = 0; j < b.size(); j++)
		FD[i][j] = false;
	if (dist(a[0], b[0]) < r + eps)
		FD[0][0] = true;
	else
		return false;
	for (int i = 0; i < a.size(); i++)
	for (int j = 0; j < b.size(); j++)
	if (dist(a[i], b[j]) < r + eps)
	{
		if (FD[i][j] == false && i && j && FD[i - 1][j - 1] && dist(a[i - 1], b[j - 1]) < r + eps)
			FD[i][j] = true;
		if (FD[i][j] == false && j && FD[i][j - 1] && dist(a[i], b[j - 1]) < r + eps)
			FD[i][j] = true;
		if (FD[i][j] == false && i && FD[i - 1][j] && dist(a[i - 1], b[j]) < r + eps)
			FD[i][j] = true;
	}

	return FD[a.size() - 1][b.size() - 1];
}

int main()
{
	vector<point> path, input;
	vector<point> addit[N];
	int dp[N][N];
	pair<int, int> par[N][N];
	double x, y;
	while (cin >> x >> y)
	{
		if (input.size() == 0 || dist(input.back(), make_pair(x, y)) + eps>rounder)
			input.push_back(make_pair(x, y));
	}

	/*cout << input.size() << endl;*/
	////add intersections
	adds(input, input, path);
	//path.assign(input.begin(), input.end());
	//cout << path.size() << endl;
	//for (int i = 0; i < path.size(); i++)
	//	cout << path[i].first << " " << path[i].second << endl;

	for (int i = 0; i < path.size(); i++)
	for (int j = 0; j < path.size(); j++)
	{
		dp[i][j] = -1;
		par[i][j] = make_pair(-1, -1);
	}
	dp[0][0] = 0;

	for (int i = 0; i < path.size(); i++)
	for (int j = 0; j < path.size(); j++)
	if (dist(path[i], path[j]) < r + eps && !(i == 0 && j == 0))
	{
		for (int ip = 0; ip <i; ip++)
		{
			//a is the simplified curve, b is the mapped part
			vector<point> a, b;
			a.push_back(path[ip]);
			a.push_back(path[i]);
			b.push_back(path[j]);
			if (dp[ip][j] != -1 && fresh(a, b) && (dp[i][j] == -1 || dp[ip][j] + 1 < dp[i][j]))
			{
				dp[i][j] = dp[ip][j] + 1;
				par[i][j] = make_pair(ip, j);
			}
		}
		for (int jp = 0; jp < j; jp++)
		{
			vector<point> a, b;
			a.push_back(path[i]);
			b.assign(path.begin() + jp, path.begin() + j + 1);
			if (dp[i][jp] != -1 && fresh(b, a) && (dp[i][j] == -1 || dp[i][j] > dp[i][jp] + 1))
			{
				dp[i][j] = dp[i][jp] + 1;
				par[i][j] = make_pair(i, jp);
			}
		}
		for (int ip = 0; ip < i; ip++)
		for (int jp = 0; jp < j; jp++)
		{
			vector<point> a, b;
			a.push_back(path[ip]);
			a.push_back(path[i]);
			b.assign(path.begin() + jp, path.begin() + j + 1);
			if (dp[ip][jp] != -1 && fresh(a, b) && (dp[i][j] == -1 || dp[i][j] > dp[ip][jp] + 1))
			{
				dp[i][j] = dp[ip][jp] + 1;
				par[i][j] = make_pair(ip, jp);
			}
		}
	}
	//input size
	//cout << path.size() << endl;
	// the length of the simplified curve
	/*cout << dp[path.size() - 1][path.size() - 1] << endl;*/
	//path
	input.resize(0);
	for (int i = path.size() - 1, j = path.size() - 1; i != -1 && j != -1;)
	{
		input.push_back(path[i]);
		pair<int, int> t = par[i][j];
		i = t.first;
		j = t.second;
	}
	for (int i = input.size() - 1; i >= 0; i--)
		cout << setprecision(6) << input[i].first << " " << input[i].second << endl;
	//cout << endl;
	//for (int i = 0; i < path.size(); i++)
	//{
	//	for (int j = 0; j < path.size(); j++)
	//		cout << setw(2) << par[i][j].first << "," << par[i][j].second << " ";
	//	cout << endl;
	//}
	//cout << endl;
	//for (int i = 0; i < path.size(); i++)
	//{
	//	for (int j = 0; j < path.size(); j++)
	//		cout << setw(2) << dp[i][j] << " ";
	//	cout << endl;
	//}

	return 0;
}