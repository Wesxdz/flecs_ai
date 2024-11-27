import requests
import datetime

def fetch_top_wikipedia_articles(top_n=10000):
    # Base URL for Wikimedia Pageviews API
    base_url = "https://wikimedia.org/api/rest_v1/metrics/pageviews/top/en.wikipedia/all-access/"

    # Use the most recent month
    today = datetime.datetime.today()
    month = f"{today.year}/{today.month:02d}"

    # Fetch data
    url = base_url + month
    response = requests.get(url)

    if response.status_code != 200:
        print(f"Error fetching data: {response.status_code}")
        print("Response text:", response.text)
        return []

    try:
        data = response.json()
    except requests.exceptions.JSONDecodeError:
        print("Error decoding JSON response.")
        print("Response text:", response.text)
        return []

    # Parse the response to extract top articles
    articles = []
    for article_data in data.get("items", [{}])[0].get("articles", []):
        # Skip "Main_Page" and "Special:" pages
        if article_data["article"] not in ("Main_Page",) and not article_data["article"].startswith("Special:"):
            articles.append(article_data["article"])
            if len(articles) >= top_n:
                break

    print(f"Fetched {len(articles)} popular article titles.")
    return articles

if __name__ == "__main__":
    top_articles = fetch_top_wikipedia_articles(top_n=10000)
    
    # Save to a file
    if top_articles:
        with open("top_wikipedia_articles.txt", "w", encoding="utf-8") as file:
            file.write("\n".join(top_articles))
        print("Top articles saved to top_wikipedia_articles.txt")
    else:
        print("No articles fetched.")
