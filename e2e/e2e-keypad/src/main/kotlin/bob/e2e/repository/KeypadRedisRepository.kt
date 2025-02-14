package bob.e2e.repository

import org.springframework.data.redis.core.RedisTemplate
import org.springframework.stereotype.Repository
import java.util.concurrent.TimeUnit

@Repository
class KeypadRedisRepository(private val redisTemplate: RedisTemplate<String, Any>) {

    fun saveKeypadData(sessionId: String, buttonValues: List<String>, hashValues: List<String>) {
        val key = "keypad:$sessionId"
        val data = buttonValues.zip(hashValues).toMap()
        redisTemplate.opsForHash<String, String>().putAll(key, data)
        redisTemplate.expire(key, 1, TimeUnit.MINUTES) // 1분 후 만료
    }

    fun getKeyHashMap(sessionId: String): Map<String, String> {
        val key = "keypad:$sessionId"
        return redisTemplate.opsForHash<String, String>().entries(key)
    }

//    fun getButtonValue(sessionId: String, hashValue: String): String? {
//        val key = "keypad:$sessionId"
//        return redisTemplate.opsForHash<String, String>().get(key, hashValue) as String?
//    }
}