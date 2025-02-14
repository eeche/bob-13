""" Database connection and session management. """
import os
from sqlalchemy import create_engine
from sqlalchemy.orm import sessionmaker, scoped_session
from sqlalchemy.ext.declarative import declarative_base
from dotenv import load_dotenv
# from config import config

# DB_PASSWORD = config['dbpassword']

# Database connection string
# DB_HOST = os.environ.get('DB_HOST', 'mariadb')
# DB_USER = os.environ.get('DB_USER', 'root')
# DB_PASSWORD = os.environ.get('DB_PASSWORD', '1234')
# DB_NAME = os.environ.get('DB_NAME', 'dbname')
# DB_CONN = f'mysql+pymysql://root:{DB_PASSWORD}@localhost:3306/chat_bot'

load_dotenv()

DB_HOST = os.getenv('DB_HOST')
DB_USER = os.getenv('DB_USER')
DB_PASSWORD = os.getenv('DB_PASSWORD')
DB_NAME = os.getenv('DB_NAME')

DB_CONN = f"mysql+pymysql://{DB_USER}:{DB_PASSWORD}@{DB_HOST}/{DB_NAME}"

class SQLAlchemy():

    def __init__(self):
        self.engine = create_engine(DB_CONN, pool_pre_ping=True, pool_size=20, max_overflow=0, pool_recycle=3600, connect_args={'connect_timeout': 10})
        self.Session = scoped_session(sessionmaker(bind=self.engine, autoflush=False, autocommit=False))

    def get_session(self):
        db = self.Session()
        try:
            yield db
        finally:
            db.close()

db = SQLAlchemy()
try:
    with db.engine.connect() as connection:
        print("Database connection successful!")
except Exception as e:
    print(f"Database connection failed: {e}")
# declarative 
Base = declarative_base() # pylint: disable=invalid-name